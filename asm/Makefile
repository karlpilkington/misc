all: hello atoi

atoi: atoi.o
	gcc -o $@ $^

atoi.o: atoi.asm
	nasm -f elf $^

hello: hello.o
	gcc -o $@ $^

hello.o: hello.asm
	nasm -f elf $^

.PHONY: clean

clean:
	rm -f *.o hello atoi
