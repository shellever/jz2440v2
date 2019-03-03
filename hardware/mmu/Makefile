objs := head.o init.o leds.o

mmu.bin: $(objs)
	arm-linux-ld -Tmmu.lds -o mmu.elf $^
	arm-linux-objcopy -O binary -S mmu.elf $@
	arm-linux-objdump -D -m arm mmu.elf > mmu.dis
	
%.o: %.c
	arm-linux-gcc -Wall -O2 -c -o $@ $<

%.o: %.S
	arm-linux-gcc -Wall -O2 -c -o $@ $<

distclean: clean
	rm -f *.bin
clean:
	rm -f *.elf *.dis *.o		

.PHONY: clean distclean

