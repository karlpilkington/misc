SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%,$(SRCS))
all: $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS) *.o
