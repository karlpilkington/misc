PREFIX=/usr/local
OBJS=libzcontrol.a zcon 
all: $(OBJS)
CFLAGS= -I. -Iinclude
#CFLAGS+=-O2
CFLAGS+=-g 
#CFLAGS+=-DZMQ_MAKE_VALGRIND_HAPPY=1

libzcontrol.a: zcontrol.o
	ar cr $@ $^

zcon: zcon.c 
	$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -o $@ $< -lzmq -lreadline 

install: $(OBJS)
	cp zcon $(PREFIX)/bin
	cp libzcontrol.a $(PREFIX)/lib
	cp zcontrol.h $(PREFIX)/include

.PHONY: clean

clean:
	rm -f *.o $(OBJS)
