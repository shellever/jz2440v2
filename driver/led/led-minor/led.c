#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/arch/regs-gpio.h>


#define CLASS_NAME    "led"
#define CHAR_DEV_NAME "led-minor"
#define MAJOR_LED     232

#define LED_ON  0
#define LED_OFF 1


static struct class *led_class;
static struct class_device *led_cls_devs[4];


static char led_status = 0x0;   // keep led status
static DECLARE_MUTEX(led_lock); // declare semaphore and initiliaze it


/* static unsigned long *gpio_va; */
/* #define GPIO_OFFSET(x) ((x) - 0x56000000) */
/* #define GPFCON (*(volatile unsigned long *)(gpio_va + GPIO_OFFSET(0x56000050))) */
/* #define GPFDAT (*(volatile unsigned long *)(gpio_va + GPIO_OFFSET(0x56000054))) */


static inline unsigned int fminor(const struct file *file)
{
    return MINOR(file->f_dentry->d_inode->i_rdev);
}

static int led_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode); // linux/fs.h
    switch (minor) {
        case 0: // /dev/led0
            s3c2410_gpio_cfgpin(S3C2410_GPF4, S3C2410_GPF4_OUTP);
            s3c2410_gpio_setpin(S3C2410_GPF4, LED_OFF);
            
            down(&led_lock);
            led_status &= ~(1 << 0);
            up(&led_lock);
            break;
        case 1: // /dev/led1
            s3c2410_gpio_cfgpin(S3C2410_GPF5, S3C2410_GPF5_OUTP);
            s3c2410_gpio_setpin(S3C2410_GPF5, LED_OFF);
            
            down(&led_lock);
            led_status &= ~(1 << 1);
            up(&led_lock);
            break;
        case 2: // /dev/led2
            s3c2410_gpio_cfgpin(S3C2410_GPF6, S3C2410_GPF6_OUTP);
            s3c2410_gpio_setpin(S3C2410_GPF6, LED_OFF);
            
            down(&led_lock);
            led_status &= ~(1 << 2);
            up(&led_lock);
            break;
        case 3: // /dev/leds
            s3c2410_gpio_cfgpin(S3C2410_GPF4, S3C2410_GPF4_OUTP);
            s3c2410_gpio_cfgpin(S3C2410_GPF5, S3C2410_GPF5_OUTP);
            s3c2410_gpio_cfgpin(S3C2410_GPF6, S3C2410_GPF6_OUTP);

            s3c2410_gpio_setpin(S3C2410_GPF4, LED_OFF);
            s3c2410_gpio_setpin(S3C2410_GPF5, LED_OFF);
            s3c2410_gpio_setpin(S3C2410_GPF6, LED_OFF);

            down(&led_lock);
            led_status = 0x0;
            up(&led_lock);
            break;
    }

    return 0;
}

static ssize_t led_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    int ret = -1;
    char status;
    int minor = fminor(file);

    switch (minor) {
        case 0: // /dev/led0
            down(&led_lock);
            status = (led_status >> 0) & 0x1;
            up(&led_lock);
            break;
        case 1: // /dev/led1
            down(&led_lock);
            status = (led_status >> 1) & 0x1;
            up(&led_lock);
            break;
        case 2: // /dev/led2
            down(&led_lock);
            status = (led_status >> 2) & 0x1;
            up(&led_lock);
            break;
        case 3: // /dev/leds
            down(&led_lock);
            status = led_status;
            up(&led_lock);
            break;
    }
    ret = copy_to_user(buff, &status, 1);   // copy led status to user
    if (ret) {
        printk(KERN_ERR "copy_to_user error\n");
        return -EINVAL;
    }

    // byte to read successfully
    return 1;
}

static ssize_t led_write(struct file *file, const char __user *buff, size_t size, loff_t *ppos)
{
    int ret = -1;
    char status;
    int minor = fminor(file);

    ret = copy_from_user(&status, buff, 1);
    if (ret) {
        printk(KERN_ERR "copy_from_user error\n");
        return -EINVAL;
    }

    if (status != LED_ON) {
        status = LED_OFF;
    }

    switch (minor) {
        case 0: // /dev/led0
            s3c2410_gpio_setpin(S3C2410_GPF4, status);
            // update led status
            if (status == LED_ON) {
                down(&led_lock);
                led_status &= ~(1 << 0);
                up(&led_lock);
            } else {
                down(&led_lock);
                led_status |= (1 << 0);
                up(&led_lock);
            }
            break;
        case 1: // /dev/led1
            s3c2410_gpio_setpin(S3C2410_GPF5, status);
            // update led status
            if (status == LED_ON) {
                down(&led_lock);
                led_status &= ~(1 << 1);
                up(&led_lock);
            } else {
                down(&led_lock);
                led_status |= (1 << 1);
                up(&led_lock);
            }
            break;
        case 2: // /dev/led2
            s3c2410_gpio_setpin(S3C2410_GPF6, status);
            // update led status
            if (status == LED_ON) {
                down(&led_lock);
                led_status &= ~(1 << 2);
                up(&led_lock);
            } else {
                down(&led_lock);
                led_status |= (1 << 2);
                up(&led_lock);
            }
            break;
        case 3: // /dev/leds
            s3c2410_gpio_setpin(S3C2410_GPF4, (status & 0x1));
            s3c2410_gpio_setpin(S3C2410_GPF5, (status & 0x1));
            s3c2410_gpio_setpin(S3C2410_GPF6, (status & 0x1));

            // update led status
            down(&led_lock);
            led_status = status;
            up(&led_lock);
            break;
    }
    
    // byte to write successfully
    return 1;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
};


static int __init led_init(void)
{
    int ret;
    int minor;

    /* gpio_va = ioremap(0x56000000, 0x100000); */
    /* if (!gpio_va) { */
        /* printk("ioremap gpio failed\n"); */
        /* return -EIO; */
    /* } */

    ret = register_chrdev(MAJOR_LED, CHAR_DEV_NAME, &led_fops);
    if (ret < 0) {
        printk("register chrdev failed\n");
        return ret;
    }

    // /sys/class/led
    led_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(led_class)) {
        printk("class create failed\n");
        return PTR_ERR(led_class);
    }

    
    // /sys/class/led/led0-led2 --mdev--> /dev/led0-led2
    for (minor = 0; minor < 3; minor++) {
        led_cls_devs[minor] = class_device_create(led_class, NULL, MKDEV(MAJOR_LED, minor), NULL, "led%d", minor);
        if (unlikely(IS_ERR(led_cls_devs[minor]))) {
            printk("create class device[%d] failed\n", minor);
            return PTR_ERR(led_cls_devs[minor]);
        }
    }
    // /sys/class/led/leds --mdev--> /dev/leds
    led_cls_devs[minor] = class_device_create(led_class, NULL, MKDEV(MAJOR_LED, minor), NULL, "leds");
    if (unlikely(IS_ERR(led_cls_devs[minor]))) {
        printk("create class device[%d] failed\n", minor);
        return PTR_ERR(led_cls_devs[minor]);
    }

    printk("led module initialized ok\n");
    return 0;
}

static void __exit led_exit(void)
{
    int minor;

    for (minor = 0; minor < 4; minor++) {
        class_device_unregister(led_cls_devs[minor]);
    }

    class_destroy(led_class);

    unregister_chrdev(MAJOR_LED, CHAR_DEV_NAME);

    /* iounmap(gpio_va); */
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 led minor driver");
MODULE_LICENSE("GPL");

