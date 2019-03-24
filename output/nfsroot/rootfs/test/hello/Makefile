hello.out: hello.o
	arm-linux-gcc -o $@ $^

hello.o: hello.c
	arm-linux-gcc -c -o $@ $^

clean:
	rm -f $(wildcard *.o)

distclean: clean
	rm -f $(wildcard *.out)

.PHONY: clean distclean

