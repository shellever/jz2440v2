#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
 
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
 
#include <asm/io.h>
#include <asm/arch/regs-nand.h>
#include <asm/arch/nand.h>


// ref
// kernel/drivers/mtd/nand/s3c2410.c
// kernel/drivers/mtd/nand/at91_nand.c


// NAND Flash contoller
struct s3c_nand_regs {
	unsigned long nfconf;       // NAND flash configuration register
	unsigned long nfcont;       // NAND flash control register
	unsigned long nfcmd;        // NAND flash command set register
	unsigned long nfaddr;       // NAND flash address set register
	unsigned long nfdata;       // NAND flash data register
	unsigned long nfeccd0;      // NAND Flash ECC 1st and 2nd register for main data read
	unsigned long nfeccd1;      // NAND Flash ECC 3nd 4th register for main data read
	unsigned long nfeccd;       // NAND flash ECC (Error Correction Code)
	unsigned long nfstat;       // NAND flash operation status register
	unsigned long nfestat0;     // NAND flash ECC Status register for I/O [7:0]
	unsigned long nfestat1;     // NAND flash ECC Status register for I/O [15:8]
	unsigned long nfmecc0;      // NAND flash ECC register for data[7:0]
	unsigned long nfmecc1;      // NAND flash ECC register for data[15:8]
	unsigned long nfsecc;       // NAND flash ECC register for I/O [15:0]
	unsigned long nfsblk;       // NAND flash programmable start block address
	unsigned long nfeblk;       // NAND flash programmable end block address
};

static struct s3c_nand_regs *s3c_nand_regs;

static struct nand_chip *s3c_nand;
static struct mtd_info *s3c_mtd;

// ref
// kernel/arch/arm/plat-s3c24x0/common-smdk.c
static struct mtd_partition s3c_nand_parts[] = {
    [0] = {
        .name   = "bootloader",
        .offset	= 0,
        .size   = 0x00040000,
    },
    [1] = {
        .name   = "params",
        .offset = MTDPART_OFS_APPEND,
        .size   = 0x00020000,
    },
    [2] = {
        .name   = "kernel",
        .offset = MTDPART_OFS_APPEND,
        .size   = 0x00200000,
    },
    [3] = {
        .name   = "root",
        .offset = MTDPART_OFS_APPEND,
        .size   = MTDPART_SIZ_FULL,
    }
};


static void s3c2440_select_chip(struct mtd_info *mtd, int chipnr)
{
	if (chipnr == -1) { // disable chip select
        s3c_nand_regs->nfcont |= (1<<1);
	} else {            // enable chip select
        s3c_nand_regs->nfcont &= ~(1<<1);
	}
}

static void s3c2440_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	if (ctrl & NAND_CLE) {
		s3c_nand_regs->nfcmd = dat;     // send command
	} else {
		s3c_nand_regs->nfaddr = dat;    // send address
	}
}

static int s3c2440_dev_ready(struct mtd_info *mtd)
{
    // bit[0] - RnB (Read-only), the status of RnB input pin
    // 0 = NAND Flash memory busy
    // 1 = NAND Flash memory ready to operate
	return (s3c_nand_regs->nfstat & (1<<0));
}

static __init int s3c_nand_init(void)
{
    struct clk *clk;

    // memory mapping
    s3c_nand_regs = ioremap(0x4E000000, sizeof(struct s3c_nand_regs));

    // allocate and initialize nand_chip structure
    s3c_nand = kzalloc(sizeof(struct nand_chip), GFP_KERNEL);

    s3c_nand->select_chip = s3c2440_select_chip;
    s3c_nand->cmd_ctrl  = s3c2440_cmd_ctrl;
    s3c_nand->IO_ADDR_R = &s3c_nand_regs->nfdata;
    s3c_nand->IO_ADDR_W = &s3c_nand_regs->nfdata;
    s3c_nand->dev_ready = s3c2440_dev_ready;
	s3c_nand->ecc.mode  = NAND_ECC_SOFT;    // error check

    // other operations about hardware
    // enable nand flash controller clock
    clk = clk_get(NULL, "nand");
    clk_enable(clk);

    // NFCONF - NAND flash configuration register
    // bit[13:12] - TACLS, CLE & ALE duration settings value (0~3)
    // Duration = HCLK x TACLS
    // HCLK = 100MHz, Duration = 0, TACLS = 0
    // bit[10:8] - TWRPH0, TWRPH0 duration setting value (0~7)
    // Duration = HCLK x (TWRPH0 + 1)
    // HCLK = 100MHz, Duration >= 12ns, TWRPH0 >= 1
    // bit[6:4] - TWRPH1, TWRPH1 duration setting value (0~7)
    // Duration = HCLK x (TWRPH1 + 1)
    // HCLK = 100MHz, Duration >= 5ns, TWRPH1 >= 0
    s3c_nand_regs->nfconf = (0<<12) | (1<<8) | (0<<4);

    // NFCONT - NAND flash control register
    // bit[1] - NAND Flash Memory nFCE signal control
    // 1 = force nFCE to high (disable chip select)
    // bit[0] - NAND flash controller operating mode
    // 1 = NAND flash controller enable
    s3c_nand_regs->nfcont = (1<<1) | (1<<0);

    // allocate and initialize mtd_info structure
    s3c_mtd = kzalloc(sizeof(struct mtd_info), GFP_KERNEL);

    s3c_mtd->owner = THIS_MODULE;
    s3c_mtd->priv = s3c_nand;

    // scan to recognize nand flash
    nand_scan(s3c_mtd, 1);

    // add mtd device or partitions
    // add_mtd_device(s3c_mtd);
    add_mtd_partitions(s3c_mtd, s3c_nand_parts, 4);

    return 0;
}

static __exit void s3c_nand_exit(void)
{
    del_mtd_partitions(s3c_mtd);
    kfree(s3c_mtd);
    kfree(s3c_nand);
    iounmap(s3c_nand_regs);
}

module_init(s3c_nand_init);
module_exit(s3c_nand_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 nand flash driver");
MODULE_LICENSE("GPL");

