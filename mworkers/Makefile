SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%,$(SRCS))
all: $(OBJS)

tworkers1: tworkers1.c
	$(CC) $(CFLAGS) -pthread -o $@ $^

.PHONY: clean

clean:
	rm -f $(OBJS) *.o
