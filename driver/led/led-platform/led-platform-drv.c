#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>


#define CHAR_DEV_NAME "led-chrdev"
#define CLASS_NAME    "led"
#define DEVICE_NAME   "led-platform"
#define LED_ON  0
#define LED_OFF 1


static struct class *led_platform_class;
static struct class_device *led_platform_class_device;

static volatile unsigned long *gpio_con = NULL;
static volatile unsigned long *gpio_dat = NULL;

static int major;
static int pin;


static int led_platform_open(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    // output
    *gpio_con &= ~(0x3 << (pin*2));
    *gpio_con |= (0x1 << (pin*2));

    // LED_OFF
    *gpio_dat |= (1 << pin);

    return 0;
}

static ssize_t led_platform_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    return 1;
}

static ssize_t led_platform_write(struct file *file, const char __user *buff, size_t size, loff_t *ppos)
{
    int val;
    int ret = -1;

    printk(KERN_DEBUG "enter %s\n", __func__);

    ret = copy_from_user(&val, buff, size);   // kernel <- user
    if (ret) {
        printk(KERN_ERR "copy_from_user error\n");
        return -EINVAL;
    }

    if (val == LED_ON) {
        *gpio_dat &= ~(1 << pin);
    } else { // if (val == LED_OFF)
        *gpio_dat |= (1 << pin);
    }

    return size;
}

static int led_platform_release(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    return 0;
}

static struct file_operations led_platform_fops = {
    .owner = THIS_MODULE,
    .open = led_platform_open,
    .read = led_platform_read,
    .write = led_platform_write,
    .release = led_platform_release,
};


static int led_platform_probe(struct platform_device *pdev)
{
    struct resource *res;

    printk(KERN_DEBUG "enter %s\n", __func__);

    // get gpio group - GPF
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    gpio_con = ioremap(res->start, res->end - res->start + 1);
    gpio_dat = gpio_con + 1;

    // get gpio number - GPF5
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    pin = res->start;


    major = register_chrdev(0, CHAR_DEV_NAME, &led_platform_fops);
    if (major < 0) {
        printk(KERN_ERR "register char device failed, major = %d\n", major);
        goto fail_register_chrdev;
    }

    // /sys/class/led
    led_platform_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(led_platform_class)) {
        printk(KERN_ERR "class create failed\n");
        goto fail_class_create;
    }
    // /sys/class/led/led-platform --mdev(busybox)--> /dev/led-platform
    led_platform_class_device = class_device_create(led_platform_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_platform_class_device)) {
        printk(KERN_ERR "class device create failed\n");
        goto fail_class_device_create;
    }

    return 0;

fail_class_device_create:
    class_destroy(led_platform_class);
fail_class_create:
    unregister_chrdev(major, CHAR_DEV_NAME);
fail_register_chrdev:
    return -EINVAL;
}

static int led_platform_remove(struct platform_device *pdev)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    class_device_unregister(led_platform_class_device);
    class_destroy(led_platform_class);

    unregister_chrdev(major, CHAR_DEV_NAME);

    iounmap(gpio_con);

    return 0;
}


struct platform_driver led_pdrv = {
    .probe = led_platform_probe,
    .remove = led_platform_remove,
    .driver = {
        .name = "led-platform",     // match for platform_device's name
    },
};

static int __init led_platform_init(void)
{
    return platform_driver_register(&led_pdrv);
}

static void __exit led_platform_exit(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    platform_driver_unregister(&led_pdrv);
}

module_init(led_platform_init);
module_exit(led_platform_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 led platform driver");
MODULE_LICENSE("GPL");

