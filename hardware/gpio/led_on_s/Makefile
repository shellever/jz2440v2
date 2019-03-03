led_on_s.bin: led_on_s.S
	arm-linux-gcc -g -c -o led_on_s.o led_on_s.S
	arm-linux-ld -Ttext 0x0000000 -g led_on_s.o -o led_on_s.elf
	arm-linux-objcopy -O binary -S led_on_s.elf led_on_s.bin

clean:
	rm -f *.dis *.elf *.o
distclean: clean
	rm -f *.bin

.PHONY: clean distclean

