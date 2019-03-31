#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>


static struct class *key_simple_class;
static struct class_device *key_simple_cls_dev;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;
volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;

int major;


static int key_simple_open(struct inode *inode, struct file *file)
{
    // input 
    // GPF0 GPF2
    *gpfcon &= ~((0x3 << (0*2)) | (0x3 << (2*2)));
    // GPG3 GPG11
    *gpgcon &= ~((0x3 << (3*2)) | (0x3 << (11*2)));

    return 0;
}

static ssize_t key_simple_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    unsigned char key_vals[4];
    int regval;
    int ret = -1;

    if (size != sizeof(key_vals)) {
        return -EINVAL;
    }

    regval = *gpfdat;
    key_vals[0] = (regval & (1 << 0)) ? 1 : 0;
    key_vals[1] = (regval & (1 << 2)) ? 1 : 0;
    regval = *gpgdat;
    key_vals[2] = (regval & (1 << 3)) ? 1 : 0;
    key_vals[3] = (regval & (1 << 11)) ? 1 : 0;

    // kernel -> user
    ret = copy_to_user(buff, key_vals, sizeof(key_vals));
    if (ret != 0) {
        printk(KERN_ERR "copy_to_user error\n");
        return -EINVAL;
    }

    return sizeof(key_vals);
}

static ssize_t key_simple_write(struct file *file, const char __user *buff, size_t size, loff_t *ppos)
{
    return 0;
}

static int key_simple_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations key_simple_fops = {
    .owner = THIS_MODULE,
    .open = key_simple_open,
    .read = key_simple_read,
    .write = key_simple_write,
    .release = key_simple_release,
};


static int __init key_simple_init(void)
{
    major = register_chrdev(0, "key-simple", &key_simple_fops);
    if (major < 0) {
        printk(KERN_ERR "register char device failed, major = %d\n", major);
        goto fail_register_chrdev;
    }

    // /sys/class/key
    key_simple_class = class_create(THIS_MODULE, "key");
    if (IS_ERR(key_simple_class)) {
        printk(KERN_ERR "class create failed\n");
        goto fail_class_create;
    }

    // /sys/class/key/key-simple --mdev(busybox)--> /dev/key-simple
    key_simple_cls_dev = class_device_create(key_simple_class, NULL, MKDEV(major, 0), NULL, "key-simple");
    if (IS_ERR(key_simple_cls_dev)) {
        printk(KERN_ERR "class device create failed\n");
        goto fail_class_device_create;
    }

    gpfcon = ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;
    gpgcon = ioremap(0x56000060, 16);
    gpgdat = gpgcon + 1;

    return 0;

fail_class_device_create:
    class_destroy(key_simple_class);
fail_class_create:
    unregister_chrdev(major, "key-simple");
fail_register_chrdev:
    return -EINVAL;
}

static void __exit key_simple_exit(void)
{
    class_device_unregister(key_simple_cls_dev);
    class_destroy(key_simple_class);
    unregister_chrdev(major, "key-simple");

    iounmap(gpfcon);
    iounmap(gpgcon);
}

module_init(key_simple_init);
module_exit(key_simple_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 key simple driver");
MODULE_LICENSE("GPL");

