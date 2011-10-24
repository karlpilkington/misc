OBJS=libzcontrol.a zcon 
all: $(OBJS)
CFLAGS= -I. -Iinclude
#CFLAGS+=-O2
CFLAGS+=-g 

libzcontrol.a: zcontrol.o
	ar cr $@ $^

zcon: zcon.c 
	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -o $@ $< -lzmq -lreadline 

.PHONY: clean

clean:
	rm -f *.o $(OBJS)
