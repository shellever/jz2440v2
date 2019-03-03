CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CFLAGS 		+= -fno-builtin-puts
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)
LDFLASG     := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

export CC LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS AR

objs := head.o init.o nand.o interrupt.o adc_ts.o serial.o main.o lib/libc.a

adc_ts.bin: $(objs)
	${LD} -Tadc_ts.lds -o adc_ts.elf $^ ${LDFLASG}
	${OBJCOPY} -O binary -S adc_ts.elf $@
	${OBJDUMP} -D -m arm adc_ts.elf > adc_ts.dis

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
	rm -f *.bin

.PHONY: clean distclean

