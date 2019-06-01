#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>


// disable by default
#define DEBUG_ENABLE


#ifdef DEBUG_ENABLE
extern int zprintk(const char *fmt, ...);
#define LOG_PREFIX          "[debug] "
#define dprintk(msg...)		zprintk(LOG_PREFIX msg)
#else
#define dprintk(msg...)
#endif


struct debug_dev {
    struct miscdevice debug_device;
};

static struct debug_dev *debug_dev;


static int debug_zprintk_open(struct inode *inode, struct file *file)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static ssize_t debug_zprintk_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static ssize_t debug_zprintk_write(struct file *file, const char __user *buff, size_t size, loff_t *ppos)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static int debug_zprintk_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    dprintk("call %s\n", __func__);
    return 0;
}

static int debug_zprintk_release(struct inode *inode, struct file *file)
{
    dprintk("call %s\n", __func__);

    return 0;
}

static struct file_operations debug_zprintk_fops = {
    .owner = THIS_MODULE,
    .open = debug_zprintk_open,
    .read = debug_zprintk_read,
    .write = debug_zprintk_write,
    .ioctl = debug_zprintk_ioctl,
    .release = debug_zprintk_release,
};


static int __init debug_zprintk_init(void)
{
    int ret = -1;

    dprintk("call %s\n", __func__);

    debug_dev = kzalloc(sizeof(struct debug_dev), GFP_KERNEL);
    if (debug_dev == NULL) {
        dprintk("%s: kmalloc failed\n", __func__);
        ret = -ENOMEM;
        goto err0;
    }
    dprintk("call %s: kmalloc success\n", __func__);

	/* register as a misc device */
	debug_dev->debug_device.minor = MISC_DYNAMIC_MINOR;
	debug_dev->debug_device.name = "debug-level";    // /dev/debug-level
	debug_dev->debug_device.fops = &debug_zprintk_fops;
	ret = misc_register(&debug_dev->debug_device);
	if (ret) {
        dprintk("%s: misc_register failed\n", __func__);
		goto err1;
    }
    dprintk("call %s: misc_register success\n", __func__);

    return 0;

err1:
    kfree(debug_dev);
err0:
    return ret;
}

static void __exit debug_zprintk_exit(void)
{
    dprintk("call %s\n", __func__);
    misc_deregister(&debug_dev->debug_device);
    kfree(debug_dev);
}

module_init(debug_zprintk_init);
module_exit(debug_zprintk_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 debug with zprintk driver");
MODULE_LICENSE("GPL");

