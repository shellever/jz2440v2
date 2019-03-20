CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS := -Wall -g

# internal SRAM ADDR for bootmode
LINK_ADDR_NAND = 0x00000000
LINK_ADDR_NOR  = 0x40000000


all: init_nand.bin init_nor.bin

init_nand.bin: init.o
	$(LD) -Ttext $(LINK_ADDR_NAND) -o init_nand.elf $^
	$(OBJCOPY) -O binary -S init_nand.elf init_nand.bin
	$(OBJDUMP) -D -m arm init_nand.elf > init_nand.dis

init_nor.bin: init.o
	$(LD) -Ttext $(LINK_ADDR_NOR) -o init_nor.elf $^
	$(OBJCOPY) -O binary -S init_nor.elf init_nor.bin
	$(OBJDUMP) -D -m arm init_nor.elf > init_nor.dis
	
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.elf *.dis *.o		

distclean: clean
	rm -f *.bin

.PHONY: all clean distclean

