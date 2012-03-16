SRCS=$(wildcard *.c)
EXES=histogram histo-test slidogram hello 
CFLAGS=-g
LDFLAGS=-lncurses

all: $(EXES)

histogram: histogram.o tpl.o

histo-test: histo-test.o tpl.o

.PHONY: clean
clean: 
	rm -f $(EXES) *.o
