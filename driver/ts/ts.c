#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <asm/plat-s3c24xx/ts.h>

#include <asm/arch/regs-adc.h>
#include <asm/arch/regs-gpio.h>


// ADC and Touch Screen Interface Special Registers
struct ts_regs {
    unsigned long adccon;   // ADC Control Register
    unsigned long adctsc;   // ADC Touch Screen Control Register
    unsigned long adcdly;   // ADC Start Delay Register
    unsigned long adcdat0;  // ADC Conversion Data Register 0
    unsigned long adcdat1;  // ADC Conversion Data Register 1
    unsigned long adcupdn;  // ADC Touch Screen Up-Down Int Check Register
};

static volatile struct ts_regs *ts_regs;
static struct input_dev *ts_input_dev;

static struct timer_list ts_timer;


static void enter_wait_pen_down_mode(void)
{
    // bit[8] - UD_SEN, detect stylus up or down status
    // - 0 = detect stylus down interrupt signal
    // - 1 = detect stylus up interrupt signal
    // bit[7] - YM_SEN, YM switch enable
    // - 0 = YM output driver disable
    // - 1 = YM output driver enable
    // bit[6] - YP_SEN, YP switch enable
    // - 0 = YP output driver enable
    // - 1 = YP output driver disable
    // bit[5] - XM_SEN, XM switch enable
    // - 0 = XM output driver disable
    // - 1 = XM output driver enable
    // bit[4] - XP_SEN, XP switch enable
    // - 0 = XP output driver enable
    // - 1 = XP output driver disable
    // bit[3] - PULL_UP, pull-up switch enable
    // - 0 = XP pull-up enable
    // - 1 = XP pull-up disable
    // bit[2] - AUTO_PST, automatically sequencing conversion of X-postion and Y-position
    // - 0 = normal ADC conversion
    // - 1 = auto sequential messurement of X-position, Y-position
    // bit[1:0] - XY_PST, manually measurement of X-position or Y-position
    // - 11 - waiting for interrupt mode
    ts_regs->adctsc = 0xd3;
}

static void enter_wait_pen_up_mode(void)
{
    ts_regs->adctsc = 0x1d3;
}

static void enter_measure_xy_mode(void)
{
    // bit[3] - PULL_UP, pull-up switch enable
    // - 1 = XP pull-up disable
    // bit[2] - AUTO_PST, automatically sequencing conversion of X-postion and Y-position
    // - 1 = auto sequential messurement of X-position, Y-position
    ts_regs->adctsc = (1<<3) | (1<<2);
}

static void start_adc(void)
{
    // bit[0]    - ENABLE_START, A/D conversion starts by enable
    // - 1 = A/D conversion starts and this bit is cleared after the startup
    ts_regs->adccon |= (1<<0);
}

static void ts_timer_function(unsigned long data)
{
    if (ts_regs->adcdat0 & (1<<15)) {
        input_report_abs(ts_input_dev, ABS_PRESSURE, 0);
        input_report_key(ts_input_dev, BTN_TOUCH, 0);
        input_sync(ts_input_dev);

        enter_wait_pen_down_mode();
    } else {
        enter_measure_xy_mode();
        start_adc();
    }
}

static irqreturn_t pen_down_up_isr(int irq, void *dev_id)
{
    // bit[15] - UPDOWN, Up or Down state of stylus at waiting for interrupt mode
    // - 0 = Stylus down state
    // - 1 = Stylus up state
    if (ts_regs->adcdat0 & (1<<15)) {
        //printk("pen up\n");
        input_report_abs(ts_input_dev, ABS_PRESSURE, 0);
        input_report_key(ts_input_dev, BTN_TOUCH, 0);
        input_sync(ts_input_dev);

        enter_wait_pen_down_mode();
    } else {
        // printk("pen down\n");
        // enter_wait_pen_up_mode();
        
        enter_measure_xy_mode();
        start_adc();
    }

    return IRQ_HANDLED;
}

static int ts_filter(int x[], int y[])
{
#define ERR_LIMIT 10

	int avr_x, avr_y;
	int del_x, del_y;

	avr_x = (x[0] + x[1])/2;
	avr_y = (y[0] + y[1])/2;

	del_x = (x[2] > avr_x) ? (x[2] - avr_x) : (avr_x - x[2]);
	del_y = (y[2] > avr_y) ? (y[2] - avr_y) : (avr_y - y[2]);

	if ((del_x > ERR_LIMIT) || (del_y > ERR_LIMIT))
		return 0;

	avr_x = (x[1] + x[2])/2;
	avr_y = (y[1] + y[2])/2;

	del_x = (x[3] > avr_x) ? (x[3] - avr_x) : (avr_x - x[3]);
	del_y = (y[3] > avr_y) ? (y[3] - avr_y) : (avr_y - y[3]);

	if ((del_x > ERR_LIMIT) || (del_y > ERR_LIMIT))
		return 0;
	
	return 1;
}

static irqreturn_t adc_isr(int irq, void *dev_id)
{
    static int cnt = 0;
    static int x[4], y[4];
    int x_avr, y_avr;

    if (ts_regs->adcdat0 & (1<<15)) {
        cnt = 0;
        enter_wait_pen_down_mode();
    } else {
        x[cnt] = ts_regs->adcdat0 & 0x3ff;
        y[cnt] = ts_regs->adcdat1 & 0x3ff;
        cnt++;
        if (cnt == 4) {
            if (ts_filter(x, y)) {
                x_avr = (x[0] + x[1] + x[2] + x[3]) / 4;
                y_avr = (y[0] + y[1] + y[2] + y[3]) / 4;

                //printk("x = %d, y = %d\n", x_avr, y_avr);
                input_report_abs(ts_input_dev, ABS_X, x_avr);
                input_report_abs(ts_input_dev, ABS_Y, y_avr);
                input_report_abs(ts_input_dev, ABS_PRESSURE, 1);
                input_report_key(ts_input_dev, BTN_TOUCH, 1);
                input_sync(ts_input_dev);
            }
            cnt = 0;
            enter_wait_pen_up_mode();

            // long press and move
            mod_timer(&ts_timer, jiffies + HZ/100);
        } else {
            enter_measure_xy_mode();
            start_adc();
        }
    }

    return IRQ_HANDLED;
}

static int __init ts_init(void)
{
    struct clk *clk;
    int ret;

    // allocate input device
    ts_input_dev = input_allocate_device();
    if (ts_input_dev == NULL) {
        ret = -ENOMEM;
        goto err0;
    }

    // name
    ts_input_dev->name = "touchscreen";

    // event
    set_bit(EV_KEY, ts_input_dev->evbit);
    set_bit(EV_ABS, ts_input_dev->evbit);

    // key
    set_bit(BTN_TOUCH, ts_input_dev->keybit);

    // 10bit ADC -> 0x3FF
    input_set_abs_params(ts_input_dev, ABS_X, 0, 0x3FF, 0, 0);
    input_set_abs_params(ts_input_dev, ABS_Y, 0, 0x3FF, 0, 0);
    input_set_abs_params(ts_input_dev, ABS_PRESSURE, 0, 1, 0, 0);

    // register input device
    ret = input_register_device(ts_input_dev);
    if (ret) {
        printk(KERN_ERR "unable to register touchscreen input device\n");
        goto err1;
    }

    // enable adc clock
    // CLKCON[15]
    clk = clk_get(NULL, "adc");
    clk_enable(clk);

    // memory mapping
    ts_regs = ioremap(0x58000000, sizeof(struct ts_regs));

    // bit[14]   - PRSCEN, A/D converter prescaler enable
    // - 0 = disable
    // - 1 = enable
    // bit[13:6] - PRSCVL, A/D converter prescaler value, data value: 0~255
    // - PCLK = 50MHz, PRSCVL = 49
    // - A/D converter freq = 50MHz / (49+1) = 1MHZ
    // - Conversion time = 1 / (1MHz / 5cycles) = 1/200KHz = 5us
    // bit[0]    - ENABLE_START, A/D conversion starts by enable
    // - 0 = no operation
    // - 1 = A/D conversion starts and this bit is cleared after the startup
    ts_regs->adccon = (1<<14) | (49<<6);

    // bit[15:0] - DELAY, ADC conversion start delay value
    // - wait for stable voltage, and then send IRQ_TC
    ts_regs->adcdly = 0xffff;

    // touch change
    ret = request_irq(IRQ_TC, pen_down_up_isr, IRQF_SAMPLE_RANDOM, "ts_pen", NULL);
    if (ret) {
        printk(KERN_ERR "request ts_pen irq failed\n");
        goto err2;
    }
    // adc conversion completely
    ret = request_irq(IRQ_ADC, adc_isr, IRQF_SAMPLE_RANDOM, "adc", NULL);
    if (ret) {
        printk(KERN_ERR "request adc irq failed\n");
        goto err3;
    }

    // init timer
    init_timer(&ts_timer);
    ts_timer.function = ts_timer_function;
    add_timer(&ts_timer);

    // Touch Screen Interface Mode
    // 1. Normal Conversion Mode
    // 2. Separate X/Y position conversion Mode
    // 3. Auto(Sequential) X/Y Position Conversion Mode
    // 4. Waiting for Interrupt Mode
    enter_wait_pen_down_mode();     // Mode 4

    return 0;

err3:
    free_irq(IRQ_TC, NULL);
err2:
    input_unregister_device(ts_input_dev);
err1:
    input_free_device(ts_input_dev);
err0:
    return ret;
}

static void __exit ts_exit(void)
{
    del_timer(&ts_timer);

    free_irq(IRQ_TC, NULL);
    free_irq(IRQ_ADC, NULL);

    iounmap(ts_regs);

    input_unregister_device(ts_input_dev);
    input_free_device(ts_input_dev);
}

module_init(ts_init);
module_exit(ts_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 touchscreen driver");
MODULE_LICENSE("GPL");

