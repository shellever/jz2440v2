CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)

export CC LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS

objs := head.o init.o nand.o serial.o main.o lib/libc.a

stdio.bin: $(objs)
	${LD} -Tstdio.lds -o stdio.elf $^
	${OBJCOPY} -O binary -S stdio.elf $@
	${OBJDUMP} -D -m arm stdio.elf > stdio.dis

.PHONY: lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o: %.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	make clean -C lib
	rm -f *.elf *.dis *.o

distclean: clean
	rm -rf *.bin

.PHONY: clean distclean

