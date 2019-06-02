#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>


// disable by default
//#define DEBUG_ENABLE


#ifdef DEBUG_ENABLE
#define dprintk(msg...)		printk(KERN_INFO msg)
#else
#define dprintk(msg...)
#endif

// ref
// drivers/i2c/chips/eeprom.c


struct at24cxx_dev {
    struct i2c_client at24cxx_client;
    struct miscdevice at24cxx_device;
};

static struct at24cxx_dev *at24cxx_dev;


static unsigned short ignore[] = {I2C_CLIENT_END};
static unsigned short normal_addr[] = {0x50, I2C_CLIENT_END}; // 0xA0>>1=0x50
//static unsigned short force_addr[] = {ANY_I2C_BUS, 0x60, I2C_CLIENT_END};
//static unsigned short *forces[] = {force_addr, NULL};

static struct i2c_client_address_data addr_data = {
    .normal_i2c = normal_addr,
    .probe = ignore,
    .ignore = ignore,
    //.forces = forces,     // found force
};

static struct i2c_driver at24cxx_driver;


static int at24cxx_open(struct inode *inode, struct file *file)
{
    //struct at24cxx_dev *at24cxx_dev;
    //struct miscdevice *misc;

    dprintk("call %s: %d,%d\n", __func__, imajor(inode), iminor(inode));

    //at24cxx_dev = container_of(file->private_data, struct at24cxx_dev, at24cxx_device);
    //misc = container_of(file->f_op, struct miscdevice, fops);
    //at24cxx_dev = container_of(misc, struct at24cxx_dev, at24cxx_device);

    file->private_data = at24cxx_dev;

    return 0;
}

static ssize_t at24cxx_read(struct file *file, char __user *buf, size_t size, loff_t * offset)
{
    int ret;
    unsigned char address;
    unsigned char data;
    struct i2c_msg msg[2];
    struct at24cxx_dev *at24cxx_dev = file->private_data;
    struct i2c_client *at24cxx_client = &at24cxx_dev->at24cxx_client;

    dprintk("call %s: size = %d\n", __func__, size);

    if (size != 1)
        return -EINVAL;

    // buf[0] = address
    // buf[1] = data
    ret = copy_from_user(&address, buf, 1);
    dprintk("call %s: copy_from_user, ret = %d\n", __func__, ret);
    if (ret) {
        dprintk(KERN_ERR "copy_from_user error\n");
        return -EINVAL;
    }

    msg[0].addr = at24cxx_client->addr; // dst
    msg[0].buf = &address;      // src
    msg[0].len = 1;             // addr
    msg[0].flags = 0;           // write

    msg[1].addr = at24cxx_client->addr; // dst
    msg[1].buf = &data;         // src
    msg[1].len = 1;             // data=1 byte
    msg[1].flags = I2C_M_RD;    // read

    ret = i2c_transfer(at24cxx_client->adapter, msg, 2);
    dprintk("call %s: i2c_transfer, ret = %d\n", __func__, ret);
    if (ret == 2) {
        ret = copy_to_user(buf, &data, 1);
        if (ret != 0) {
            dprintk(KERN_ERR "copy_to_user error\n");
            return -EINVAL;
        }
        return 1;
    } 

    return -EIO;
}

static ssize_t at24cxx_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    int ret;
    unsigned char val[2];
    struct i2c_msg msg[1];
    struct at24cxx_dev *at24cxx_dev = file->private_data;
    struct i2c_client *at24cxx_client = &at24cxx_dev->at24cxx_client;

    dprintk("call %s: size = %d\n", __func__, size);

    if (size != 2)
        return -EINVAL;

    // buf[0] = address
    // buf[1] = data
    ret = copy_from_user(val, buf, 2);
    dprintk("call %s: copy_from_user, ret = %d\n", __func__, ret);
    if (ret) {
        dprintk(KERN_ERR "copy_from_user error\n");
        return -EINVAL;
    }

    msg[0].addr = at24cxx_client->addr; // dst
    msg[0].buf = val;   // src
    msg[0].len = 2;     // addr+data=2 bytes
    msg[0].flags = 0;   // write

    ret = i2c_transfer(at24cxx_client->adapter, msg, 1);
    dprintk("call %s: i2c_transfer, ret = %d\n", __func__, ret);
    if (ret == 1)
        return 2;

    return -EIO;
}

static struct file_operations at24cxx_fops = {
    .owner = THIS_MODULE,
    .open = at24cxx_open,
    .read = at24cxx_read,
    .write = at24cxx_write,
};


// called by i2c_probe when i2c device found
static int at24cxx_detect(struct i2c_adapter *adapter, int address, int kind)
{
    int ret;
    struct i2c_client *new_client;
    //struct at24cxx_dev *at24cxx_dev; // internal device specific data

    dprintk("call %s: address = %d, kind = %d\n", __func__, address, kind);

    /* allocate the at24cxx driver information structure */
    at24cxx_dev = kzalloc(sizeof(struct at24cxx_dev), GFP_KERNEL);
    if (at24cxx_dev == NULL) {
        dprintk("failed to allocate memory for module data\n");
        ret = -ENOMEM;
        goto err_exit;
    }

    // attach i2c client
    new_client = &at24cxx_dev->at24cxx_client;
    new_client->addr = address;
    new_client->adapter = adapter;
    new_client->driver = &at24cxx_driver;
    strcpy(new_client->name, "at24cxx");

    // tell the I2C layer a new client has arrived
    ret = i2c_attach_client(new_client);
    if (ret) {
        dprintk("i2c_attach_client failed\n");
        goto err_free;
    }

    /* register as a misc device */
    at24cxx_dev->at24cxx_device.minor = MISC_DYNAMIC_MINOR;
    at24cxx_dev->at24cxx_device.name = "at24cxx";    // /dev/at24cxx
    at24cxx_dev->at24cxx_device.fops = &at24cxx_fops;
    ret = misc_register(&at24cxx_dev->at24cxx_device);
    if (ret) {
        dprintk("misc_register failed\n");
        goto err_detach;
    }

    i2c_set_clientdata(new_client, at24cxx_dev);

    return 0;

err_detach:
    i2c_detach_client(new_client);
err_free:
    kfree(at24cxx_dev);
err_exit:
    return ret;
}

static int at24cxx_attach_adapter(struct i2c_adapter *adapter)
{
    dprintk("call %s\n", __func__);
    return i2c_probe(adapter, &addr_data, at24cxx_detect);
}

static int at24cxx_detach_client(struct i2c_client *client)
{
    int ret;
    struct at24cxx_dev *at24cxx_dev;

    dprintk("call %s\n", __func__);

    at24cxx_dev = i2c_get_clientdata(client);

    misc_deregister(&at24cxx_dev->at24cxx_device);

    ret = i2c_detach_client(client);
    if (ret) {
        return ret;
    }

    kfree(at24cxx_dev);

    return 0;
}

static struct i2c_driver at24cxx_driver = {
    .driver = {
        .name = "at24cxx",
    },
    .attach_adapter = at24cxx_attach_adapter,
    .detach_client = at24cxx_detach_client,
};

static int __init at24cxx_init(void)
{
    dprintk("call %s\n", __func__);
    return i2c_add_driver(&at24cxx_driver);
}

static void __exit at24cxx_exit(void)
{
    dprintk("call %s\n", __func__);
    i2c_del_driver(&at24cxx_driver);
}

module_init(at24cxx_init);
module_exit(at24cxx_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 i2c of at24cxx driver");
MODULE_LICENSE("GPL");

