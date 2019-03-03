CFLAGS := -Wall -O2
CFLAGS += -fno-builtin-puts

objs := head.o init.o serial.o main.o

uart.bin: $(objs)
	arm-linux-ld -Tuart.lds -o uart.elf $^
	arm-linux-objcopy -O binary -S uart.elf $@
	arm-linux-objdump -D -m arm uart.elf > uart.dis
	
%.o: %.c
	arm-linux-gcc $(CFLAGS) -c -o $@ $<

%.o: %.S
	arm-linux-gcc $(CFLAGS) -c -o $@ $<

distclean: clean
	rm -f *.bin
clean:
	rm -f *.elf *.dis *.o

.PHONY: clean distclean

