#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/input.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/hardware.h>
#include <asm/arch/regs-gpio.h>


// ref
// kernel/drivers/input/keyboard/gpio-keys.c


struct pin_desc {
    int irq;
    char *name;
    unsigned int pin;
    unsigned int key_val;
};

// pressed key value by default
struct pin_desc pin_desc_tab[4] = {
    {IRQ_EINT0,  "S2", S3C2410_GPF0,  KEY_L},
    {IRQ_EINT2,  "S3", S3C2410_GPF2,  KEY_S},
    {IRQ_EINT11, "S4", S3C2410_GPG3,  KEY_ENTER},
    {IRQ_EINT19, "S5", S3C2410_GPG11, KEY_LEFTSHIFT},
};
#define NR_KEYS (sizeof(pin_desc_tab)/sizeof(pin_desc_tab[0]))

static struct pin_desc *irq_pd;
static struct input_dev *key_input_dev;
static struct timer_list key_timer;


// get key value in interrupt function
static irqreturn_t key_isr(int irq, void *dev_id)
{
    irq_pd = (struct pin_desc *)dev_id;
    //mod_timer(&key_timer, jiffies+HZ/100);  // 10ms
    mod_timer(&key_timer, jiffies+msecs_to_jiffies(10));  // 10ms

    return IRQ_HANDLED;
}

static void key_timer_func(unsigned long data)
{
    struct pin_desc *pindesc = irq_pd;
    if (!pindesc)
        return;

    if (s3c2410_gpio_getpin(pindesc->pin)) {
        input_event(key_input_dev, EV_KEY, pindesc->key_val, 0); // no pressed
    } else {
        input_event(key_input_dev, EV_KEY, pindesc->key_val, 1); // pressed
    }

    input_sync(key_input_dev);  // start report
}


static int __init key_input_init(void)
{
    int i, ret;

    printk(KERN_DEBUG "enter %s\n", __func__);

    // request irq
    for (i = 0; i < NR_KEYS; i++) {
        ret = request_irq(pin_desc_tab[i].irq, key_isr, IRQT_BOTHEDGE, 
                pin_desc_tab[i].name, &pin_desc_tab[i]);
        if (ret) {
            printk(KERN_ERR "request irq failed\n");
            goto fail;
        }
    }


    // timer init
    init_timer(&key_timer);
    key_timer.function = key_timer_func;
    //key_input.expires = 0;    // will be modified in interrupt function
    add_timer(&key_timer);


    // input device init
    key_input_dev = input_allocate_device();
    if (!key_input_dev)
        return -ENOMEM;

    // name
    // key_input_dev->name = "keys";

    // event
    set_bit(EV_KEY, key_input_dev->evbit);
    set_bit(EV_REP, key_input_dev->evbit);

    // key
    set_bit(KEY_L, key_input_dev->keybit);
    set_bit(KEY_S, key_input_dev->keybit);
    set_bit(KEY_ENTER, key_input_dev->keybit);
    set_bit(KEY_LEFTSHIFT, key_input_dev->keybit);

    // register input device
    ret = input_register_device(key_input_dev);
    if (ret) {
        printk(KERN_ERR "unable to register key input device\n");
        goto fail;
    }

    return 0;

fail:
    for (i = i - 1; i >= 0; i--)
        free_irq(pin_desc_tab[i].irq, &pin_desc_tab[i]);

    del_timer(&key_timer);
    input_free_device(key_input_dev);

    return ret;
}

static void __exit key_input_exit(void)
{
    int i;
    printk(KERN_DEBUG "enter %s\n", __func__);

    for (i = 0; i < NR_KEYS; i++) {
        free_irq(pin_desc_tab[i].irq, &pin_desc_tab[i]);
    }

    del_timer(&key_timer);

    input_unregister_device(key_input_dev);
    input_free_device(key_input_dev);
}

module_init(key_input_init);
module_exit(key_input_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 key input driver");
MODULE_LICENSE("GPL");

