LIB = libcontrolport.a
all: $(LIB)

SRCS = controlport.c 
OBJS = $(patsubst %.c,%.o,$(SRCS))

$(OBJS): $(SRCS)

$(LIB): $(OBJS)
	ar cr $@ $^

.PHONY: clean

clean:
	rm -f *.o $(LIB)
