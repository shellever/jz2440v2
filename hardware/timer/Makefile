objs := head.o init.o interrupt.o main.o

timer.bin: $(objs)
	arm-linux-ld -Ttimer.lds -o timer.elf $^
	arm-linux-objcopy -O binary -S timer.elf $@
	arm-linux-objdump -D -m arm timer.elf > timer.dis
	
%.o: %.c
	arm-linux-gcc -Wall -O2 -c -o $@ $<

%.o: %.S
	arm-linux-gcc -Wall -O2 -c -o $@ $<

distclean: clean
	rm -f *.bin
clean:
	rm -f *.elf *.dis *.o

.PHONY: clean distclean

