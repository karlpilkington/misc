SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%,$(SRCS))
all: $(OBJS)

$(OBJS): $(SRCS)

.PHONY: clean

clean:
	rm -f $(OBJS) *.o
