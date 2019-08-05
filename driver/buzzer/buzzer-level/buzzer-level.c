#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <asm/hardware.h>
#include <asm/gpio.h>
#include <asm/uaccess.h>
#include <asm/arch/regs-gpio.h>


#define DEBUG_ENABLE        1
#define LOG_PREFIX          "[debug] "
#define dprintk(msg...)     do { \
    if (DEBUG_ENABLE) printk(KERN_INFO LOG_PREFIX msg); \
} while (0)

// linux-2.6.22.6/drivers/net/spider_net.h
#define pr_err(fmt, arg...) \
    printk(KERN_ERR fmt, ##arg)


#define LOW     0
#define HIGH    1

#define BUZZER_LEVEL_CTRL_MODE HIGH

#if BUZZER_LEVEL_CTRL_MODE == HIGH
#define BUZZER_ON   HIGH
#define BUZZER_OFF  LOW
#else
#define BUZZER_ON   LOW
#define BUZZER_OFF  HIGH
#endif

#define BUZZER_IOC_MAGIC        'b'
#define BUZZER_IOC_WR_BEEP_CTRL _IOW(BUZZER_IOC_MAGIC, 1, unsigned int)
#define BUZZER_DEVICE_NAME      "buzzer-level"  // /dev/buzzer-level

#define BUZZER_BEEP_MODE_ON     0x01
#define BUZZER_BEEP_MODE_OFF    0x02
#define BUZZER_BEEP_MODE_MS     0x03

// EINT20/GPG12
// nGCS3/GPA14
//#define BUZZER_IO   S3C2410_GPG12
#define BUZZER_IO   S3C2410_GPA14


struct buzzer_dev {
    struct miscdevice buzzer_device;
    struct timer_list buzzer_timer;
    int ctrl_gpio;
};

static struct buzzer_dev *buzzer_dev;


static int buzzer_level_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int buzzer_level_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int mode;
    int duration = 0;
    unsigned int buf[2];
    int val;
    int ret;

    switch (cmd) {
        case BUZZER_IOC_WR_BEEP_CTRL:
            ret = copy_from_user(buf, (__user int *)arg, sizeof(buf));   // kernel <- user
            if (ret) {
                printk(KERN_ERR "copy_from_user error\n");
                return -EINVAL;
            }
            dprintk("call %s: mode = %d, duration = %d\n", __func__, buf[0], buf[1]);

            mode = buf[0];
            if (mode == BUZZER_BEEP_MODE_ON) {
                val = BUZZER_ON;
            } else if (mode == BUZZER_BEEP_MODE_OFF) {
                val = BUZZER_OFF;
            } else if (mode == BUZZER_BEEP_MODE_MS) {
                val = BUZZER_ON;
                duration = buf[1];
            } else {
                printk(KERN_ERR "BUZZER_IOC_WR_BEEP_CTRL: error mode\n");
                return -EINVAL;
            }
            break;
        default:
            return -EINVAL;
    }

    //s3c2410_gpio_setpin(buzzer_dev->ctrl_gpio, val);
    gpio_set_value(buzzer_dev->ctrl_gpio, val);
    if (mode == BUZZER_BEEP_MODE_MS) {
        dprintk("call %s: jiffies = %lu, HZ = %d\n", __func__, jiffies, HZ);
        mod_timer(&buzzer_dev->buzzer_timer, jiffies+msecs_to_jiffies(duration));  // unit: ms
    }

    return 0;
}

static struct file_operations buzzer_level_fops = {
    .owner = THIS_MODULE,
    .open = buzzer_level_open,
    .ioctl = buzzer_level_ioctl,
};

static void buzzer_timer_func(unsigned long data)
{
    dprintk("call %s: jiffies = %lu, HZ = %d\n", __func__, jiffies, HZ);
    //s3c2410_gpio_setpin(buzzer_dev->ctrl_gpio, BUZZER_OFF);
    gpio_set_value(buzzer_dev->ctrl_gpio, BUZZER_OFF);
}


static int __init buzzer_level_init(void)
{
    int ret = -1;

    dprintk("call %s\n", __func__);

    buzzer_dev = kzalloc(sizeof(struct buzzer_dev), GFP_KERNEL);
    if (buzzer_dev == NULL) {
        pr_err("%s : kmalloc failed\n", __func__);
        ret = -ENOMEM;
        goto err0;
    }
    dprintk("call %s: kmalloc success\n", __func__);

    //s3c2410_gpio_cfgpin(BUZZER_IO, S3C2410_GPIO_OUTPUT);

    /* ctrl_gpio */
    ret = gpio_request(BUZZER_IO, "ctrl_gpio");
    if (ret) {
        pr_err("%s : gpio_request failed\n", __func__);
        goto err1;
    }
    buzzer_dev->ctrl_gpio = BUZZER_IO;
    //s3c2410_gpio_cfgpin(buzzer_dev->ctrl_gpio, S3C2410_GPIO_OUTPUT);
    //s3c2410_gpio_setpin(buzzer_dev->ctrl_gpio, BUZZER_OFF);     // off by default
    gpio_direction_output(buzzer_dev->ctrl_gpio, BUZZER_OFF);   // off by default
    dprintk("call %s: config and set gpio success\n", __func__);

    /* init timer */
    init_timer(&buzzer_dev->buzzer_timer);
    buzzer_dev->buzzer_timer.function = buzzer_timer_func;
    //buzzer_dev->buzzer_timer.expires = 0;
    add_timer(&buzzer_dev->buzzer_timer);
    dprintk("call %s: init timer success\n", __func__);

    /* register as a misc device */
    buzzer_dev->buzzer_device.minor = MISC_DYNAMIC_MINOR;
    buzzer_dev->buzzer_device.name = BUZZER_DEVICE_NAME;
    buzzer_dev->buzzer_device.fops = &buzzer_level_fops;
    ret = misc_register(&buzzer_dev->buzzer_device);
    if (ret) {
        pr_err("%s : misc_register failed\n", __func__);
        goto err2;
    }
    dprintk("call %s: misc_register success\n", __func__);

    return 0;

err2:
    del_timer(&buzzer_dev->buzzer_timer);
    gpio_free(buzzer_dev->ctrl_gpio);
err1:
    kfree(buzzer_dev);
err0:
    return ret;
}

static void __exit buzzer_level_exit(void)
{
    dprintk("call %s\n", __func__);
    misc_deregister(&buzzer_dev->buzzer_device);
    del_timer(&buzzer_dev->buzzer_timer);
    gpio_free(buzzer_dev->ctrl_gpio);
    kfree(buzzer_dev);
}

module_init(buzzer_level_init);
module_exit(buzzer_level_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 buzzer level driver");
MODULE_LICENSE("GPL");

