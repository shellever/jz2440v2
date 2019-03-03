sdram.bin: head.S leds.c
	arm-linux-gcc -c -o head.o head.S
	arm-linux-gcc -c -o leds.o leds.c
	arm-linux-ld -Ttext 0x30000000 head.o leds.o -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D -m arm sdram.elf > sdram.dis

distclean: clean
	rm -f *.bin
clean:
	rm -f *.elf *.dis *.o

.PHONY: distclean clean

