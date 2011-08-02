SRCS=$(wildcard *.c)
EXES=$(patsubst %.c,%,$(SRCS))
LDFLAGS=-lncurses

all: $(EXES)

.PHONY: clean
clean: 
	rm -f $(EXES) *.o
