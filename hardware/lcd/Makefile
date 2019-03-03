CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)

export CC LD AR OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS

objs := head.o init.o nand.o interrupt.o serial.o lcddrv.o framebuffer.o lcdlib.o main.o lib/libc.a

lcd.bin: $(objs)
	${LD} -Tlcd.lds -o lcd.elf $^
	${OBJCOPY} -O binary -S lcd.elf $@
	${OBJDUMP} -D -m arm lcd.elf > lcd.dis

.PHONY: lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o: %.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

distclean: clean
	rm -f *.bin
clean:
	make clean -C lib
	rm -f *.elf *.dis *.o

.PHONY: clean distclean

