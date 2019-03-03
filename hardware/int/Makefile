objs := head.o init.o interrupt.o main.o

int.bin: $(objs)
	arm-linux-ld -Ttext 0x00000000 -o int.elf $^
	arm-linux-objcopy -O binary -S int.elf $@
	arm-linux-objdump -D -m arm int.elf > int.dis
	
%.o: %.c
	arm-linux-gcc -Wall -O2 -c -o $@ $<

%.o: %.S
	arm-linux-gcc -Wall -O2 -c -o $@ $<

distclean: clean
	rm -f *.bin

clean:
	rm -f *.elf *.dis *.o

.PHONY: clean distclean

