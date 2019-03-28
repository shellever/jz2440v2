#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>


#define CHAR_DEV_NAME "led-chrdev"
#define CLASS_NAME    "led"
#define DEVICE_NAME   "led-simple"
#define LED_ON  0
#define LED_OFF 1


static struct class *led_simple_class;
static struct class_device *led_simple_class_device;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

int major;


static int led_simple_open(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    // output
    *gpfcon &= ~((0x3 << (4*2)) | (0x3 << (5*2)) | (0x3 << (6*2)));
    *gpfcon |= ((0x1 << (4*2)) | (0x1 << (5*2)) | (0x1 << (6*2)));

    // LED_OFF
    *gpfdat |= ((1 << 4) | (1 << 5) | (1 << 6));

    return 0;
}

static ssize_t led_simple_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    return 1;
}

static ssize_t led_simple_write(struct file *file, const char __user *buff, size_t size, loff_t *ppos)
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
        *gpfdat &= ~((1 << 4) | (1 << 5) | (1 << 6));
    } else { // if (val == LED_OFF)
        *gpfdat |= ((1 << 4) | (1 << 5) | (1 << 6));
    }

    return size;
}

static int led_simple_release(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    return 0;
}

static struct file_operations led_simple_fops = {
    .owner = THIS_MODULE,
    .open = led_simple_open,
    .read = led_simple_read,
    .write = led_simple_write,
    .release = led_simple_release,
};


static int __init led_simple_init(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    major = register_chrdev(0, CHAR_DEV_NAME, &led_simple_fops);
    if (major < 0) {
        printk(KERN_ERR "register char device failed, major = %d\n", major);
        goto fail_register_chrdev;
    }

    // /sys/class/led
    led_simple_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(led_simple_class)) {
        printk(KERN_ERR "class create failed\n");
        goto fail_class_create;
    }
    // /sys/class/led/led-simple --mdev(busybox)--> /dev/led-simple
    led_simple_class_device = class_device_create(led_simple_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_simple_class_device)) {
        printk(KERN_ERR "class device create failed\n");
        goto fail_class_device_create;
    }

    gpfcon = (volatile unsigned long *) ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;

    return 0;

fail_class_device_create:
    class_destroy(led_simple_class);
fail_class_create:
    unregister_chrdev(major, CHAR_DEV_NAME);
fail_register_chrdev:
    return -EINVAL;
}

static void __exit led_simple_exit(void)
{
    printk(KERN_DEBUG "enter %s\n", __func__);

    class_device_unregister(led_simple_class_device);
    class_destroy(led_simple_class);

    unregister_chrdev(major, CHAR_DEV_NAME);

    iounmap(gpfcon);
}

module_init(led_simple_init);
module_exit(led_simple_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 led simple driver");
MODULE_LICENSE("GPL");

