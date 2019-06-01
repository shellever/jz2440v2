#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>


// ref
// kernel/drivers/mtd/maps/physmap.c


static struct map_info *s3c_nor_map;
static struct mtd_info *s3c_nor_mtd;

static struct mtd_partition s3c_nor_parts[] = {
	[0] = {
        .name   = "bootloader_nor",
		.offset	= 0,
        .size   = 0x00040000,       // 256KB
	},
	[1] = {
        .name   = "root_nor",
        .offset = MTDPART_OFS_APPEND,
        .size   = MTDPART_SIZ_FULL, // 2MB - 256KB
	}
};


static __init int s3c_nor_init(void)
{
    // 1. allocate map_info struct
    s3c_nor_map = kzalloc(sizeof(struct map_info), GFP_KERNEL);

    // 2. initialize map_info struct
    s3c_nor_map->name = "s3c_nor";
    s3c_nor_map->phys = 0;          // physical base address
    s3c_nor_map->size = 0x1000000;  // nor flash capacity >= sizeof(nor) 2MB=0x00200000
    s3c_nor_map->bankwidth = 2;     // 8*2=16bits
    s3c_nor_map->virt = ioremap(s3c_nor_map->phys, s3c_nor_map->size); // virtual address
    simple_map_init(s3c_nor_map);   // init map_info

    // 3. do map probe to regconize flash
    printk("use cfi_probe\n");
    s3c_nor_mtd = do_map_probe("cfi_probe", s3c_nor_map);
    if (!s3c_nor_mtd) {
        printk("use jedec_probe\n");
        s3c_nor_mtd = do_map_probe("jedec_probe", s3c_nor_map);
    }

    if (!s3c_nor_mtd) {
        iounmap(s3c_nor_map->virt);
        kfree(s3c_nor_map);
        return -EIO;
    }

    // 4. add mtd partitions
    add_mtd_partitions(s3c_nor_mtd, s3c_nor_parts, 2);

    return 0;
}

static __exit void s3c_nor_exit(void)
{
    del_mtd_partitions(s3c_nor_mtd);
    iounmap(s3c_nor_map->virt);
    kfree(s3c_nor_map);
}

module_init(s3c_nor_init);
module_exit(s3c_nor_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 nor flash driver");
MODULE_LICENSE("GPL");

