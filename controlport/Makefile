
LIB = libcontrolport.a
SRCS = controlport.c tpl.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
EXES = app1

all: $(LIB) $(EXES)

$(OBJS): $(SRCS)

$(LIB): $(OBJS)
	ar cr $@ $^

$(EXES): 
	$(CC) $(CFLAGS) -o $@ $@.c -L. -lcontrolport

.PHONY: clean

clean:
	rm -f *.o $(LIB) $(EXES)
