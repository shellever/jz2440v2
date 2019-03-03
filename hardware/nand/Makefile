objs := head.o init.o nand.o main.o

nand.bin: $(objs)
	arm-linux-ld -Tnand.lds	-o nand.elf $^
	arm-linux-objcopy -O binary -S nand.elf $@
	arm-linux-objdump -D -m arm nand.elf > nand.dis

%.o: %.c
	arm-linux-gcc -Wall -c -O2 -o $@ $<

%.o: %.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<

distclean: clean
	rm -f *.bin
clean:
	rm -f *.dis *.elf *.o

.PHONY: clean distclean

