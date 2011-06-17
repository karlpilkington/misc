CFLAGS = -g
OBJS = mkprob entropy1
MATH = -lm

all: $(OBJS)

entropy1: entropy1.c
	$(CC) $(CFLAGS) -o $@ $< $(MATH)

clean:
	rm -f *.o $(OBJS)
