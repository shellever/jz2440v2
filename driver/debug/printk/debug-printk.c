#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>


// disable by default
#define DEBUG_ENABLE


#ifdef DEBUG_ENABLE
#define LOG_PREFIX          "[debug] "
#define dprintk(msg...)		printk(KERN_INFO LOG_PREFIX msg)
#else
#define dprintk(msg...)
#endif


// linux-2.6.22.6/drivers/net/spider_net.h
#define pr_err(fmt, arg...) \
    printk(KERN_ERR fmt, ##arg)


struct debug_dev {
    struct miscdevice debug_device;
};

static struct debug_dev *debug_dev;


static int debug_printk_open(struct inode *inode, struct file *file)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static ssize_t debug_printk_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static ssize_t debug_printk_write(struct file *file, const char __user *buff, size_t size, loff_t *ppos)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static int debug_printk_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static int debug_printk_release(struct inode *inode, struct file *file)
{
    dprintk("call %s\n", __func__);

    return 0;
}

static struct file_operations debug_printk_fops = {
    .owner = THIS_MODULE,
    .open = debug_printk_open,
    .read = debug_printk_read,
    .write = debug_printk_write,
    .ioctl = debug_printk_ioctl,
    .release = debug_printk_release,
};


static int __init debug_printk_init(void)
{
    int ret = -1;

    dprintk("call %s\n", __func__);

    debug_dev = kzalloc(sizeof(struct debug_dev), GFP_KERNEL);
    if (debug_dev == NULL) {
		pr_err("%s : kmalloc failed\n", __func__);
        ret = -ENOMEM;
        goto err0;
    }
    dprintk("call %s: kmalloc success\n", __func__);

	/* register as a misc device */
	debug_dev->debug_device.minor = MISC_DYNAMIC_MINOR;
	debug_dev->debug_device.name = "debug-level";    // /dev/debug-level
	debug_dev->debug_device.fops = &debug_printk_fops;
	ret = misc_register(&debug_dev->debug_device);
	if (ret) {
		pr_err("%s : misc_register failed\n", __func__);
		goto err1;
    }
    dprintk("call %s: misc_register success\n", __func__);

    return 0;

err1:
    kfree(debug_dev);
err0:
    return ret;
}

static void __exit debug_printk_exit(void)
{
    dprintk("call %s\n", __func__);
    misc_deregister(&debug_dev->debug_device);
    kfree(debug_dev);
}

module_init(debug_printk_init);
module_exit(debug_printk_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 debug with printk driver");
MODULE_LICENSE("GPL");

