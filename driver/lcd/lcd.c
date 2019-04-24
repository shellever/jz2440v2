#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>
#include <asm/arch/regs-lcd.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/fb.h>


// jz2440v2 lcd 4.3inch 480x272

// LCD Controller special registers
struct lcd_regs {
    unsigned long lcdcon1;      // LCD Control 1 Register
    unsigned long lcdcon2;      // LCD Control 2 Register
    unsigned long lcdcon3;      // LCD Control 3 Register
    unsigned long lcdcon4;      // LCD Control 4 Register
    unsigned long lcdcon5;      // LCD Control 5 Register
    unsigned long lcdsaddr1;    // FRAME Buffer Start Address 1 Register
    unsigned long lcdsaddr2;    // FRAME Buffer Start Address 2 Register
    unsigned long lcdsaddr3;    // FRAME Buffer Start Address 3 Register
    unsigned long redlut;       // RED Lookup Table Register
    unsigned long greenlut;     // GREEN Lookup Table Register
    unsigned long bluelut;      // BLUE Lookup Table Register
    unsigned long reserved[9];  // Reserved for future compatibility
    unsigned long dithmode;     // Dithering Mode Register
    unsigned long tpal;         // Temp Palette Register
    unsigned long lcdintpnd;    // LCD Interrupt Pending Register
    unsigned long lcdsrcpnd;    // LCD Source Pending Register
    unsigned long lcdintmsk;    // LCD Interrupt Mask Register
    unsigned long lpcsel;       // TCON Contrl Register
};

static volatile struct lcd_regs *lcd_regs;
static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;

static struct fb_info *lcd_fb_info;
static u32 pseudo_palette[16];


static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
    chan &= 0xffff;
    chan >>= 16 - bf->length;
    return chan << bf->offset;
}

static int lcd_fb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info)
{
    unsigned int val;

    if (regno > 16)
        return 1;

    // RGB565
    val  = chan_to_field(red, &info->var.red);
    val |= chan_to_field(green, &info->var.green);
    val |= chan_to_field(blue, &info->var.blue);

    pseudo_palette[regno] = val;

    return 0;
}

static struct fb_ops lcd_fb_ops = {
    .owner = THIS_MODULE,
    .fb_setcolreg = lcd_fb_setcolreg,
    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
};

static int __init lcd_init(void)
{
    // 1. allocate fb_info
    lcd_fb_info = framebuffer_alloc(0, NULL);

    // 2. parameters configuration
    // 2.1 configure fix parameters
    strcpy(lcd_fb_info->fix.id, "mylcd");
    lcd_fb_info->fix.smem_len = 480*272*16/8;
    lcd_fb_info->fix.type = FB_TYPE_PACKED_PIXELS;
    lcd_fb_info->fix.visual = FB_VISUAL_TRUECOLOR;
    lcd_fb_info->fix.line_length = 480*2;

    // 2.3 configure variable parameters
    lcd_fb_info->var.xres = 480;
    lcd_fb_info->var.yres = 272;
    lcd_fb_info->var.xres_virtual = 480;
    lcd_fb_info->var.yres_virtual = 272;
    lcd_fb_info->var.bits_per_pixel = 16;

    // RGB: 565
    lcd_fb_info->var.red.offset = 11;
    lcd_fb_info->var.red.length = 5;
    lcd_fb_info->var.green.offset = 6;
    lcd_fb_info->var.green.length = 6;
    lcd_fb_info->var.blue.offset = 0;
    lcd_fb_info->var.blue.length = 5;

    lcd_fb_info->var.activate = FB_ACTIVATE_NOW;
    
    // 2.4 configure fb operations
    lcd_fb_info->fbops = &lcd_fb_ops;

    // 2.5 configure palette and others
    lcd_fb_info->pseudo_palette = pseudo_palette;
    lcd_fb_info->screen_size = 480*272*16/8;

    // 3. gpio and register configuration
    // 3.1 configure gpio for lcd
    gpbcon = ioremap(0x56000010, 8);
    gpbdat = gpbcon + 1;
    gpccon = ioremap(0x56000020, 4);
    gpdcon = ioremap(0x56000030, 4);
    gpgcon = ioremap(0x56000060, 4);

    // gpio for lcd
    // VD[7:0], LCDVF[2:0], VM, VFRAME, VLINE, VCLK, LEND
    *gpccon = 0xaaaaaaaa;
    // VD[23:8]
    *gpdcon = 0xaaaaaaaa;

    // GPB0 for backlight contorl
    *gpbcon &= ~(0x3<<0);   // output 
    *gpbcon |= (1<<0);
    *gpbdat &= ~(1<<0);     // turn off

    // GPG4 for LCD_PWREN
    *gpgcon |= (0x3<<8);    // LCD_PWREN 

    // 3.2 configure lcd registers
    // memory mapping for lcd
    lcd_regs = ioremap(0x4D000000, sizeof(struct lcd_regs));

    // bit[17:8] - CLKVAL, determine the rates of VCLK and CLKVAL[9:0]
    // - TFT: VCLK = HCLK / [(CLKVAL+1) x 2]
    // - VCLK = 10MHz, HCLK = 100MHz => CLKVAL = 4
    // bit[6:5]  - PNRMODE, select the display mode
    // - 11 = TFT LCD panel
    // bit[4:1]  - BPPMODE, select the BPP (Bits Per Pixel) mode
    // - 1100 = 16 bpp for TFT
    // bit[0]    - ENVID, LCD video output and the logic enable/disable
    // - 0 = disable the video output and the LCD control signal
    // - 1 = enable the video output and the LCD control signal
    lcd_regs->lcdcon1 = (4<<8) | (3<<5) | (0x0c<<1);

    // bit[31:24] - VBPD
    // - TFT: vertial back porch 
    // bit[23:14] - LINEVAL
    // - TFT/STN: vertical size of LCD panel
    // bit[13:6]  - VFPD
    // - TFT: vertical front porch
    // bit[5:0]   - VSPW
    // - TFT: vertical sync pulse width
    lcd_regs->lcdcon2 = (1<<24) | (271<<14) | (1<<6) | (9<<0);

    // bit[25:19] - HBPD
    // - TFT: horizontal back porch
    // bit[18:8]  - HOZVAL
    // - TFT/STN: horizontal size of LCD panel
    // bit[7:0]   - HFPD
    // - TFT: horizontal front porch
    lcd_regs->lcdcon3 = (1<<19) | (479<<8) | (1<<0);

    // bit[7:0] - HSPW
    // - TFT: horizontal sync pulse width
    lcd_regs->lcdcon4 = (40<<0);

    // bit[11] - FRM565
    // - TFT: select the format of 16 bpp output video data
    // - 1 = 5:6:5 Format
    // bit[10] - INVVCLK
    // - STN/TFT: control the polarity of the VCLK active edge
    // - 0 = the video data is fetched at VCLK falling edge 
    // bit[9]  - INVVLINE
    // - STN/TFT: indicate the VLINE/HSYNC pulse polarity
    // - 1 = inverted
    // bit[8]  - INVVFRAME
    // - STN/TFT: indicate the VFRAME/VSYNC pulse polarity
    // - 1 = inverted
    // bit[6]  - INVVDEN
    // - TFT: indicate the VDEN signal polarity
    // - 0 = normal, 1 = inverted
    // bit[3]  - PWREN
    // - STN/TFT: LCD_PWREN output signal enable/disable
    // - 0 = disable PWREN signal
    // - 1 = enable PWREN signal
    // bit[1]  - BSWP
    // - STN/TFT: byte swap control bit
    // - 0 = swap disable, 1 = swap enable
    // bit[0]  - HWSWAP
    // - STN/TFT: half-word swap control bit
    // - 0 = swap disable, 1 = swap enable
    lcd_regs->lcdcon5 = (1<<11) | (0<<10) | (1<<9) | (1<<8) | (1<<0);

    // 3.3 allocate framebuffer
    lcd_fb_info->screen_base = dma_alloc_writecombine(NULL, lcd_fb_info->fix.smem_len,  (dma_addr_t *)&lcd_fb_info->fix.smem_start, GFP_KERNEL);
    // bit[29:21] - LCDBANK, indicate A[30:22] of the bank location for the video buffer in the system memory
    // bit[20:0]  - LCDBASEU
    // - For single scan LCD: indicate A[21:1] of the start address of the LCD frame buffer
    lcd_regs->lcdsaddr1 = (lcd_fb_info->fix.smem_start >> 1) & ~(3 << 30);
    // bit[20:0] - LCDBASEL
    // - For single scan LCD: indicate A[21:1] of the end address of the LCD frame buffer
    lcd_regs->lcdsaddr2 = ((lcd_fb_info->fix.smem_start + lcd_fb_info->fix.smem_len) >> 1) & 0x1fffff;
    // bit[21:11] - OFFSIZE, virtual screen offset size (the number of half words)
    // bit[10:0] - PAGEWIDTH, virtual screen page width (the number of half words)
    lcd_regs->lcdsaddr3 = (480*16/16);

    // 3.4 configure to enable lcd
    // - 1 = enable the video output and the LCD control signal
    lcd_regs->lcdcon1 |= (1<<0);
    // - 1 = enable PWREN signal
    lcd_regs->lcdcon5 |= (1<<3);
    // turn on backlight
    *gpbdat |= (1<<0);
    
    // 4. register fb_info
    register_framebuffer(lcd_fb_info);
    return 0;
}

static void __exit lcd_exit(void)
{
    unregister_framebuffer(lcd_fb_info);

    lcd_regs->lcdcon1 &= ~(1<<0);
    lcd_regs->lcdcon5 &= ~(1<<3);
    *gpbdat &= ~(1<<0);

    dma_free_writecombine(NULL, lcd_fb_info->fix.smem_len, lcd_fb_info->screen_base, lcd_fb_info->fix.smem_start);

    iounmap(lcd_regs);
    iounmap(gpbcon);
    iounmap(gpccon);
    iounmap(gpdcon);
    iounmap(gpgcon);

    framebuffer_release(lcd_fb_info);
}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 lcd driver");
MODULE_LICENSE("GPL");

