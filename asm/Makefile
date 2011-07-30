EXE = hello atoi gethost
all: $(EXE)

gethost: gethost.o
	gcc -o $@ $^

gethost.o: gethost.asm
	nasm -f elf $^

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
	rm -f *.o $(EXE)
