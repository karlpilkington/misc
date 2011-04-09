SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%,$(SRCS))
all: $(OBJS)

threads_wqueue: threads_wqueue.c
	$(CC) $(CFLAGS) -pthread -o $@ $^

.PHONY: clean

clean:
	rm -f $(OBJS) *.o
