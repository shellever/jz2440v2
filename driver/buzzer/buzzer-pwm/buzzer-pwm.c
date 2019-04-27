#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/io.h>

#include <asm/hardware.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/regs-timer.h>


#define DEVICE_NAME "pwm"
#define PWM_IOCTL_STOP     0
#define PWM_IOCTL_SET_FREQ 1

static struct semaphore lock;

// freq: pclk/50/16/65536 ~ pclk/50/16
// if pclk = 50MHz, freq is 1Hz to 62500Hz
// human ear: 20Hz~20000Hz
static void pwm_set_freq(unsigned long freq)
{
    unsigned long tcon;
    unsigned long tcnt;
    unsigned long tcfg1;
    unsigned long tcfg0;

    struct clk *clk_p;
    unsigned long pclk;

    // set GPB0 as tout0, pwm output
    s3c2410_gpio_cfgpin(S3C2410_GPB0, S3C2410_GPB0_TOUT0);

    tcon = __raw_readl(S3C2410_TCON);
    tcfg0 = __raw_readl(S3C2410_TCFG0);
    tcfg1 = __raw_readl(S3C2410_TCFG1);

    // prescaler = 50
    tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;
    tcfg0 |= (50 - 1);

    // mux = 1/16
    tcfg1 &= ~S3C2410_TCFG1_MUX0_MASK;
    tcfg1 |= S3C2410_TCFG1_MUX0_DIV16;

    __raw_writel(tcfg1, S3C2410_TCFG1);
    __raw_writel(tcfg0, S3C2410_TCFG0);

    clk_p = clk_get(NULL, "pclk");
    pclk  = clk_get_rate(clk_p);
    tcnt  = (pclk/50/16)/freq;

    __raw_writel(tcnt, S3C2410_TCNTB(0));
    __raw_writel(tcnt/2, S3C2410_TCMPB(0));

    tcon &= ~0x1f;
    tcon |= 0xb;    // disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
    __raw_writel(tcon, S3C2410_TCON);

    tcon &= ~2;                         // clear manual update bit
    __raw_writel(tcon, S3C2410_TCON);
}

static void pwm_stop(void)
{
    s3c2410_gpio_cfgpin(S3C2410_GPB0, S3C2410_GPIO_OUTPUT);
    s3c2410_gpio_setpin(S3C2410_GPB0, 0);
}

static int buzzer_pwm_open(struct inode *inode, struct file *file)
{
    if (!down_trylock(&lock))
        return 0;
    else
        return -EBUSY;
}

static int buzzer_pwm_close(struct inode *inode, struct file *file)
{
    pwm_stop();
    up(&lock);

    return 0;
}

static int buzzer_pwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case PWM_IOCTL_SET_FREQ:
            if (arg == 0)
                return -EINVAL;
            pwm_set_freq(arg);
            break;
        case PWM_IOCTL_STOP:
            pwm_stop();
            break;
    }

    return 0;
}


static struct file_operations buzzer_pwm_fops = {
    .owner = THIS_MODULE,
    .open = buzzer_pwm_open,
    .release = buzzer_pwm_close,
    .ioctl = buzzer_pwm_ioctl,
};

static struct miscdevice buzzer_pwm_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &buzzer_pwm_fops,
};

static int __init buzzer_pwm_init(void)
{
    int ret;
    init_MUTEX(&lock);
    ret = misc_register(&buzzer_pwm_dev);

    return ret;
}

static void __exit buzzer_pwm_exit(void)
{
    misc_deregister(&buzzer_pwm_dev);
}

module_init(buzzer_pwm_init);
module_exit(buzzer_pwm_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 buzzer pwm driver");
MODULE_LICENSE("GPL");

