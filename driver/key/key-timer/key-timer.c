#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>


#define CHAR_DEV_NAME "key-timer"
#define CLASS_NAME    "key"
#define DEVICE_NAME   "key-timer"


static struct class *key_timer_class;
static struct class_device *key_timer_class_device;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;
volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;

int major;

static DECLARE_WAIT_QUEUE_HEAD(key_waitqueue);
static volatile int ev_press = 0;   // press event flag

static struct fasync_struct *key_timer_queue;

static DECLARE_MUTEX(key_lock);     // semaphore lock

static struct timer_list key_timer;

struct pin_desc {
    int irq;
    char *name;
    unsigned int pin;
    unsigned int key_val;
};

// pressed key value by default
struct pin_desc pin_desc_tab[4] = {
    {IRQ_EINT0, "S2", S3C2410_GPF0, 0x01},
    {IRQ_EINT2, "S3", S3C2410_GPF2, 0x02},
    {IRQ_EINT11, "S4", S3C2410_GPG3, 0x03},
    {IRQ_EINT19, "S5", S3C2410_GPG11, 0x04},
};
#define NR_KEYS (sizeof(pin_desc_tab)/sizeof(pin_desc_tab[0]))

static unsigned char key_val;
static struct pin_desc *irq_pd;


// get key value in interrupt function
static irqreturn_t key_isr(int irq, void *dev_id)
{
    irq_pd = (struct pin_desc *)dev_id;
    mod_timer(&key_timer, jiffies+HZ/100);  // 10ms

    return IRQ_HANDLED;
}


static int key_timer_open(struct inode *inode, struct file *file)
{
    int i, ret;

    printk(KERN_DEBUG "enter %s\n", __func__);

    // check if non block
    if (file->f_flags & O_NONBLOCK) {
        if (down_trylock(&key_lock))
            return -EBUSY;
    } else {
        down(&key_lock);
    }

    for (i = 0; i < NR_KEYS; i++) {
        ret = request_irq(pin_desc_tab[i].irq, key_isr, IRQT_BOTHEDGE, 
                pin_desc_tab[i].name, &pin_desc_tab[i]);
        if (ret) {
            printk(KERN_ERR "request irq failed\n");
            break;
        }
    }

    if (ret) {
        for (i = i - 1; i >= 0; i--)
            free_irq(pin_desc_tab[i].irq, &pin_desc_tab[i]);

        return -EBUSY;
    }

    return 0;
}

static ssize_t key_timer_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    int ret;

    //printk(KERN_DEBUG "enter %s\n", __func__);

    if (size != sizeof(key_val))
        return -EINVAL;

    // check if non block
    if (file->f_flags & O_NONBLOCK) {
        if (!ev_press)
            return -EAGAIN;
    } else {
        wait_event_interruptible(key_waitqueue, ev_press);
    }

    ret = copy_to_user(buff, &key_val, sizeof(key_val));
    if (ret) {
        printk(KERN_ERR "copy_to_user failed\n");
        return -EINVAL;
    }
    ev_press = 0;   // reset for next time

    return sizeof(key_val);
}

static unsigned int key_timer_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;

    poll_wait(file, &key_waitqueue, wait);  // no sleep here

    if (ev_press)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

static int key_timer_fasync(int fd, struct file *file, int on)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    return fasync_helper(fd, file, on, &key_timer_queue);
}

static int key_timer_release(struct inode *inode, struct file *file)
{
    int i;

    printk(KERN_DEBUG "enter %s\n", __func__);

    for (i = 0; i < NR_KEYS; i++) {
        free_irq(pin_desc_tab[i].irq, &pin_desc_tab[i]);
    }

    up(&key_lock);

    return 0;
}

static struct file_operations key_timer_fops = {
    .owner = THIS_MODULE,
    .open = key_timer_open,
    .read = key_timer_read,
    .poll = key_timer_poll,
    .fasync = key_timer_fasync,
    .release = key_timer_release,
};

static void key_timer_func(unsigned long data)
{
    struct pin_desc *pindesc = irq_pd;
    if (!pindesc)
        return;

    if (s3c2410_gpio_getpin(pindesc->pin)) {
        key_val = 0x80 | pindesc->key_val;  // no pressed
    } else {
        key_val = pindesc->key_val;         // pressed
    }

    ev_press = 1;
    wake_up_interruptible(&key_waitqueue);  // wake up

    kill_fasync(&key_timer_queue, SIGIO, POLL_IN);    // send fasync msg
}


static int __init key_timer_init(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    major = register_chrdev(0, CHAR_DEV_NAME, &key_timer_fops);
    if (major < 0) {
        printk(KERN_ERR "register char device failed, major = %d\n", major);
        goto fail_register_chrdev;
    }

    // /sys/class/led
    key_timer_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(key_timer_class)) {
        printk(KERN_ERR "class create failed\n");
        goto fail_class_create;
    }
    // /sys/class/led/led-simple --mdev(busybox)--> /dev/led-simple
    key_timer_class_device = class_device_create(key_timer_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(key_timer_class_device)) {
        printk(KERN_ERR "class device create failed\n");
        goto fail_class_device_create;
    }

    // memory mapping
    gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;
    gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
    gpgdat = gpgcon + 1;

    // timer init
    init_timer(&key_timer);
    key_timer.function = key_timer_func;
    //key_timer.expires = 0;    // will be modified in interrupt function
    add_timer(&key_timer);

    return 0;

fail_class_device_create:
    class_destroy(key_timer_class);
fail_class_create:
    unregister_chrdev(major, CHAR_DEV_NAME);
fail_register_chrdev:
    return -EINVAL;
}

static void __exit key_timer_exit(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    class_device_unregister(key_timer_class_device);
    class_destroy(key_timer_class);

    unregister_chrdev(major, CHAR_DEV_NAME);

    del_timer(&key_timer);

    iounmap(gpfcon);
    iounmap(gpgcon);
}

module_init(key_timer_init);
module_exit(key_timer_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 key timer driver");
MODULE_LICENSE("GPL");

