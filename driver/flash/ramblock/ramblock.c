#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/wait.h>

#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>

#include <asm/system.h>
#include <asm/uaccess.h>


// ref
// drivers/block/xd.c
// drivers/block/z2ram.c


static struct gendisk *ramblock_disk;
static request_queue_t *ramblock_queue;

static int major;
static DEFINE_SPINLOCK(ramblock_lock);

#define SECTOR_SIZE             (512)
#define RAMBLOCK_SIZE           (1024*1024)   // 1MB
#define RAMBLOCK_DISK_CAPACITY  (RAMBLOCK_SIZE/SECTOR_SIZE)
static unsigned char *ramblock_buff;


// for fdisk command
static int ramblock_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
    // capacity = heads * cylinders * sectors * 512
    geo->heads = 2;
    geo->cylinders = 32;
    geo->sectors = RAMBLOCK_SIZE/2/32/512;

    return 0;
}

static void do_ramblock_request(request_queue_t *q)
{
    //static int r_cnt = 0;
    //static int w_cnt = 0;

    struct request *req;
    unsigned long offset;
    unsigned long length;

    while ((req = elv_next_request(q)) != NULL) {
        offset = req->sector * 512;
        length = req->current_nr_sectors * 512;

        if (rq_data_dir(req) == READ) {
            //printk("call %s, r_cnt = %d\n", __func__, ++r_cnt);
            memcpy(req->buffer, ramblock_buff+offset, length);
        } else {
            //printk("call %s, w_cnt = %d\n", __func__, ++w_cnt);
            memcpy(ramblock_buff+offset, req->buffer, length);
        }

        end_request(req, 1);
    }
}

static struct block_device_operations ramblock_fops = {
    .owner = THIS_MODULE,
    .getgeo = ramblock_getgeo,
};


static __init int ramblock_init(void)
{
    int ret;

    // 1. allocate gendisk struct
    ramblock_disk = alloc_disk(16);
    if (!ramblock_disk) {
        ret = -ENOMEM;
        goto out_disk;
    }

    // 2. setup ramdisk attributes
    // 2.1 initialize queue with callback and lock
    ramblock_queue = blk_init_queue(do_ramblock_request, &ramblock_lock);
    if (!ramblock_queue) {
        ret = -ENOMEM;
        goto out_queue;
    }
    ramblock_disk->queue = ramblock_queue;

    // 2.2 set major, minor, disk_name, fops, capacity and so on
    major = register_blkdev(0, "ramblock");     // cat /proc/devices
    if (major < 0) {
        ret = -EBUSY;
        goto out_blkdev;
    }
    ramblock_disk->major = major;
    ramblock_disk->first_minor = 0;
    sprintf(ramblock_disk->disk_name, "ramblock");
    ramblock_disk->fops = &ramblock_fops;
    set_capacity(ramblock_disk, RAMBLOCK_SIZE / 512);

    // 3. buffer
    ramblock_buff = kzalloc(RAMBLOCK_SIZE, GFP_KERNEL);
    if (ramblock_buff == NULL) {
        ret = -ENOMEM;
        goto out_buff;
    }

    // 4. add disk
    add_disk(ramblock_disk);

    return 0;

out_buff:
    unregister_blkdev(major, "ramblock");
out_blkdev:
    blk_cleanup_queue(ramblock_queue);
out_queue:
    put_disk(ramblock_disk);
out_disk:
    return ret;
}

static __exit void ramblock_exit(void)
{
    del_gendisk(ramblock_disk);
    kfree(ramblock_buff);
    unregister_blkdev(major, "ramblock");
    blk_cleanup_queue(ramblock_queue);
    put_disk(ramblock_disk);
}

module_init(ramblock_init);
module_exit(ramblock_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 ramblock driver");
MODULE_LICENSE("GPL");

