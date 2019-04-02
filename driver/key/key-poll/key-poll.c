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


#define CHAR_DEV_NAME "key-poll"
#define CLASS_NAME    "key"
#define DEVICE_NAME   "key-poll"


static struct class *key_poll_class;
static struct class_device *key_poll_class_device;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;
volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;

int major;

static DECLARE_WAIT_QUEUE_HEAD(key_waitqueue);
static volatile int ev_press = 0;   // press event flag

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


// get key value in interrupt function
static irqreturn_t key_isr(int irq, void *dev_id)
{
    struct pin_desc *pindesc = (struct pin_desc *)dev_id;
    if (s3c2410_gpio_getpin(pindesc->pin)) {
        key_val = 0x80 | pindesc->key_val;  // no pressed
    } else {
        key_val = pindesc->key_val;         // pressed
    }

    ev_press = 1;
    wake_up_interruptible(&key_waitqueue);  // wake up

    return IRQ_HANDLED;
}


static int key_poll_open(struct inode *inode, struct file *file)
{
    int i, ret;

    printk(KERN_DEBUG "enter %s\n", __func__);

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

static ssize_t key_poll_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    int ret;

    //printk(KERN_DEBUG "enter %s\n", __func__);

    if (size != sizeof(key_val))
        return -EINVAL;

    wait_event_interruptible(key_waitqueue, ev_press);  // waiting available

    ret = copy_to_user(buff, &key_val, sizeof(key_val));
    if (ret) {
        printk(KERN_ERR "copy_to_user failed\n");
        return -EINVAL;
    }
    ev_press = 0;

    return sizeof(key_val);
}

static unsigned int key_poll_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;

    poll_wait(file, &key_waitqueue, wait);  // no sleep here

    if (ev_press)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

static int key_poll_release(struct inode *inode, struct file *file)
{
    int i;

    printk(KERN_DEBUG "enter %s\n", __func__);

    for (i = 0; i < NR_KEYS; i++) {
        free_irq(pin_desc_tab[i].irq, &pin_desc_tab[i]);
    }

    return 0;
}

static struct file_operations key_poll_fops = {
    .owner = THIS_MODULE,
    .open = key_poll_open,
    .read = key_poll_read,
    .poll = key_poll_poll,
    .release = key_poll_release,
};


static int __init key_poll_init(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    major = register_chrdev(0, CHAR_DEV_NAME, &key_poll_fops);
    if (major < 0) {
        printk(KERN_ERR "register char device failed, major = %d\n", major);
        goto fail_register_chrdev;
    }

    // /sys/class/led
    key_poll_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(key_poll_class)) {
        printk(KERN_ERR "class create failed\n");
        goto fail_class_create;
    }
    // /sys/class/led/led-simple --mdev(busybox)--> /dev/led-simple
    key_poll_class_device = class_device_create(key_poll_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(key_poll_class_device)) {
        printk(KERN_ERR "class device create failed\n");
        goto fail_class_device_create;
    }

    gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;
    gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
    gpgdat = gpgcon + 1;

    return 0;

fail_class_device_create:
    class_destroy(key_poll_class);
fail_class_create:
    unregister_chrdev(major, CHAR_DEV_NAME);
fail_register_chrdev:
    return -EINVAL;
}

static void __exit key_poll_exit(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    class_device_unregister(key_poll_class_device);
    class_destroy(key_poll_class);

    unregister_chrdev(major, CHAR_DEV_NAME);

    iounmap(gpfcon);
    iounmap(gpgcon);
}

module_init(key_poll_init);
module_exit(key_poll_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 key poll driver");
MODULE_LICENSE("GPL");

