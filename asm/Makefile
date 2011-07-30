all: hello

hello: hello.o
	gcc -o $@ $^

hello.o: hello.asm
	nasm -f elf $^

.PHONY: clean

clean:
	rm -f *.o hello
