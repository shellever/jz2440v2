#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/dma-mapping.h>

#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>


#define CMD_MEM_CPY_NO_DMA 0
#define CMD_MEM_CPY_DMA    1

#define BUFFER_SIZE (512*1024)  // 512 kB

#define DMA0_BASE_ADDR 0x4B000000
#define DMA1_BASE_ADDR 0x4B000040
#define DMA2_BASE_ADDR 0x4B000080
#define DMA3_BASE_ADDR 0x4B0000C0


// DMA registers
struct dma_regs {
	unsigned long disrc;        // DMA initial source register
	unsigned long disrcc;       // DMA initial source control register
	unsigned long didst;        // DMA initial destination register
	unsigned long didstc;       // DMA initial destination control register
	unsigned long dcon;         // DMA control register
	unsigned long dstat;        // DMA status register
	unsigned long dcsrc;        // DMA current source register
	unsigned long dcdst;        // DMA current destination register
	unsigned long dmasktrig;    // DMA mask trigger register
};

static volatile struct dma_regs *dma_regs;

static char *src;
static u32 src_phys;

static char *dst;
static u32 dst_phys;

static struct class *dma_class;
static int major = 0;

static volatile int ev_dma = 0;
static DECLARE_WAIT_QUEUE_HEAD(dma_waitq);


static int dma_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int i;

    memset(src, 0xAA, BUFFER_SIZE);
    memset(dst, 0x55, BUFFER_SIZE);

    switch (cmd) {
        case CMD_MEM_CPY_NO_DMA:
            for (i = 0; i < BUFFER_SIZE; i++)
                dst[i] = src[i];

            if (memcmp(src, dst, BUFFER_SIZE) == 0)
                printk("MEM_CPY_NO_DMA OK\n");
            else
                printk("MEM_CPY_NO_DMA ERROR\n");

            break;
        case CMD_MEM_CPY_DMA:
            ev_dma = 0;

            // bit[30:0] - S_ADDR, base address (start address) of source data to transfer
            dma_regs->disrc = src_phys;

            // bit[1] - LOC, bit 1 is used to select the location of source
            // 0 = the source is in the system bus (AHB)
            // bit[0] - INC, bit 0 is used to select the address increment
            // 0 = increment
            dma_regs->disrcc = (0<<1)|(0<<0);

            // bit[30:0] - D_ADDR, base address (start address) of destination for the transfer
            dma_regs->didst = dst_phys;

            // bit[2] - CHK_INT, select interrupt occurrence time when auto reload is setting
            // 0 = interrupt will occur when TC reaches 0
            // bit[1] - LOC, bit 1 is used to select the location of destination
            // 0 = the destination is in the system bus (AHB)
            // bit[0] - INC, bit 0 is used to select the address increment
            // 0 = increment
            dma_regs->didstc = (0<<2)|(0<<1)|(0<<0);

            // bit[30] - SYNC, select DREQ/DACK synchronization
            // 1 = DREQ and DACK are synchronized to HCLK (AHB clock)
            // bit[29] - INT, enable/disable the interrupt setting for CURR_TC (terminal count)
            // 1 = interrupt request is generated when all the transfer is done
            // bit[28] - TSZ, select the transfer size of an atomic transfer
            // 0 = a unit transfer is preformed
            // bit[27] - SERVMODE, select the service mode between single service mode and whole service mode
            // 1 = whole service mode is selected in which one request gets atomic transfer to be repeated until the transfer count reaches to 0.
            //     in this mode, additional request are not required.
            // bit[23] - SWHW_SEL, select the DMA source between software (S/W request mode) and hardware (H/W request mode)
            // 0 = S/W request mode is selected and DMA is triggered by setting SW_TRIG bit of DMASKTRIG control register
            // bit[21:20] - DSZ, data size to be transferred
            // 00 = byte
            // bit[19:0] - TC, initial transfer count (or transfer beat)
            // Note that the actual number of bytes that are transferred is computed by the following equation: DSZ x TSZ x TC.
            dma_regs->dcon = (1<<30)|(1<<29)|(0<<28)|(1<<27)|(0<<23)|(0<<20)|(BUFFER_SIZE<<0);

            // bit[1] - ON_OFF, DMA channel on/off bit
            // 1 = DMA channel is turned on and the DMA request is handled
            // bit[0] - SW_TRIG, trigger the DMA channel is S/W request mode
            // 1 = it requests a DMA operation to this controller
            dma_regs->dmasktrig = (1<<1)|(1<<0);

            wait_event_interruptible(dma_waitq, ev_dma);    // sleep it

            if (memcmp(src, dst, BUFFER_SIZE) == 0)
                printk("MEM_CPY_DMA OK\n");
            else
                printk("MEM_CPY_DMA ERROR\n");

            break;
    }
    return 0;
}

static struct file_operations dma_fops = {
    .owner = THIS_MODULE,
    .ioctl = dma_ioctl,
};

static irqreturn_t dma_irq(int irq, void *devid)
{
    ev_dma = 1;
    wake_up_interruptible(&dma_waitq);  // wake up queue
    return IRQ_HANDLED;
}

static __init int dma_init(void)
{
    if (request_irq(IRQ_DMA3, dma_irq, 0, "s3c_dma", NULL)) {
        printk("can't request_irq for DMA\n");
        return -EBUSY;
    }

    // allocate buffer for dma using
    src = dma_alloc_writecombine(NULL, BUFFER_SIZE, &src_phys, GFP_KERNEL);
    if (src == NULL) {
        printk("cannot allocate buffer for dma of src\n");
        return -ENOMEM;
    }

    dst = dma_alloc_writecombine(NULL, BUFFER_SIZE, &dst_phys, GFP_KERNEL);
    if (dst == NULL) {
        dma_free_writecombine(NULL, BUFFER_SIZE, src, src_phys);
        printk("cannot allocate buffer for dma of dst\n");
        return -ENOMEM;
    }

    major = register_chrdev(0, "s3c_dma", &dma_fops);
    dma_class = class_create(THIS_MODULE, "s3c_dma");
    class_device_create(dma_class, NULL, MKDEV(major, 0), NULL, "dma"); // /dev/dma

    dma_regs = ioremap(DMA3_BASE_ADDR, sizeof(struct dma_regs));

    return 0;
}

static __exit void dma_exit(void)
{
    iounmap(dma_regs);

    class_device_destroy(dma_class, MKDEV(major, 0));
    class_destroy(dma_class);
    unregister_chrdev(major, "s3c_dma");

    dma_free_writecombine(NULL, BUFFER_SIZE, src, src_phys);
    dma_free_writecombine(NULL, BUFFER_SIZE, dst, dst_phys);

    free_irq(IRQ_DMA3, NULL);
}

module_init(dma_init);
module_exit(dma_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 dma driver");
MODULE_LICENSE("GPL");

