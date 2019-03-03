key_led.bin: crt0.S key_led.c
	arm-linux-gcc -g -c -o crt0.o crt0.S
	arm-linux-gcc -g -c -o key_led.o key_led.c
	arm-linux-ld -Ttext 0x0000000 -g crt0.o key_led.o -o key_led.elf
	arm-linux-objcopy -O binary -S key_led.elf key_led.bin
	arm-linux-objdump -D -m arm key_led.elf > key_led.dis

clean:
	rm -f *.dis *.elf *.o
distclean: clean
	rm -f *.bin

.PHONY: clean distclean

