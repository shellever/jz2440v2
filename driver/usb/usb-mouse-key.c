#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

#define DEBUG_ENABLE    0

// reference
// kernel/drivers/hid/usbhid/usbmouse.c
//
// using usb mouse as key

// id_table -> probe -> usb_fill_int_urb 
//      -interrupt-> usb_mouse_key_irq -> input_event

static char *usb_buff;
static dma_addr_t usb_buff_phys;
static int len;
static struct urb *uk_urb;

static struct input_dev *uk_input_dev;


static struct usb_device_id usb_mouse_key_id_table[] = {
    { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
        USB_INTERFACE_PROTOCOL_MOUSE) },
    //{USB_DEVICE(0x1234,0x5678)},  // specified vendorid and productid
    { }	/* Terminating entry */
};

static void usb_mouse_key_irq(struct urb *urb)
{
    static unsigned char pre_val;
#if DEBUG_ENABLE
    // print key code and analysis it
    int i;
    static int cnt = 0;
    printk("data cnt %d: ", ++cnt);
    for (i = 0; i < len; i++) {
        printk("%02x ", usb_buff[i]);
    }
    printk("\n");
#endif

    // usb_buff[0]
    // bit[0] - Left
    // bit[1] - Right
    // bit[3] - Middle
    // 1 - press down
    // 0 - release
    if ((pre_val & (1<<0)) != (usb_buff[0] & (1<<0))) { // sync event while toggling
        input_event(uk_input_dev, EV_KEY, KEY_L, (usb_buff[0] & (1<<0)) ? 1: 0);
        input_sync(uk_input_dev);
    }

    if ((pre_val & (1<<1)) != (usb_buff[0] & (1<<1))) {
        input_event(uk_input_dev, EV_KEY, KEY_S, (usb_buff[0] & (1<<1)) ? 1: 0);
        input_sync(uk_input_dev);
    }

    if ((pre_val & (1<<2)) != (usb_buff[0] & (1<<2))) {
        input_event(uk_input_dev, EV_KEY, KEY_ENTER, (usb_buff[0] & (1<<2)) ? 1: 0);
        input_sync(uk_input_dev);
    }
    pre_val = usb_buff[0];  // save for next time

    // submit urb again for next times
    usb_submit_urb(uk_urb, GFP_KERNEL);
}

static int usb_mouse_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    struct usb_device *dev;
    struct usb_host_interface *interface;
    struct usb_endpoint_descriptor *endpoint;
    int pipe;

    printk(KERN_INFO "enter %s\n", __func__);

    // input device
    uk_input_dev = input_allocate_device();

    uk_input_dev->name = "mouse-keys";

    set_bit(EV_KEY, uk_input_dev->evbit);
    set_bit(EV_REP, uk_input_dev->evbit);

    set_bit(KEY_L, uk_input_dev->keybit);
    set_bit(KEY_S, uk_input_dev->keybit);
    set_bit(KEY_ENTER, uk_input_dev->keybit);

    input_register_device(uk_input_dev);


    // usb device
	dev = interface_to_usbdev(intf);
#if DEBUG_ENABLE
    printk("bcdUSB    = %x\n", dev->descriptor.bcdUSB);         // 200
    printk("idVendor  = 0x%04X\n", dev->descriptor.idVendor);   // 0x24AE
    printk("idProduct = 0x%04X\n", dev->descriptor.idProduct);  // 0x1100
#endif
    interface = intf->cur_altsetting;
    endpoint = &interface->endpoint[0].desc;

    // data transmission
    // src
    pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
    // len
    len = endpoint->wMaxPacketSize;
    // dst
    usb_buff = usb_buffer_alloc(dev, len, GFP_ATOMIC, &usb_buff_phys);

    // allocate urb (usb request block)
    uk_urb = usb_alloc_urb(0, GFP_KERNEL);
    // fill interrupt urb
    usb_fill_int_urb(uk_urb, dev, pipe, usb_buff, len, usb_mouse_key_irq, NULL, endpoint->bInterval);
    // configure urb using dma transfer
    uk_urb->transfer_dma = usb_buff_phys;
    uk_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    // submit urb
    usb_submit_urb(uk_urb, GFP_KERNEL);

    return 0;
}

static void usb_mouse_key_disconnect(struct usb_interface *intf)
{
    struct usb_device *dev = interface_to_usbdev(intf);
    printk(KERN_INFO "enter %s\n", __func__);

    usb_kill_urb(uk_urb);   // usb_submit_urb
    usb_free_urb(uk_urb);   // usb_alloc_urb
    usb_buffer_free(dev, len, usb_buff, usb_buff_phys); // usb_buffer_alloc

    input_unregister_device(uk_input_dev);
    input_free_device(uk_input_dev);
}


static struct usb_driver usb_mouse_key_driver = {
    .name = "usb_mouse_key",
    .id_table = usb_mouse_key_id_table,
    .probe = usb_mouse_key_probe,
    .disconnect = usb_mouse_key_disconnect,
};

static int usb_mouse_key_init(void)
{
    return usb_register(&usb_mouse_key_driver);
}

static void usb_mouse_key_exit(void)
{
    usb_deregister(&usb_mouse_key_driver);
}

module_init(usb_mouse_key_init);
module_exit(usb_mouse_key_exit);

MODULE_LICENSE("GPL");

