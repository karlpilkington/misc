CFLAGS = -g
OBJS = 
ASCIIDOC=/usr/bin/asciidoc
DOC = Unix_domain_sockets.html

all: $(DOC)

entropy1: entropy1.c
	$(CC) $(CFLAGS) -o $@ $< $(MATH)

Unix_domain_sockets.html: Unix_domain_sockets.txt
	if [ -x $(ASCIIDOC) ]; then $(ASCIIDOC) $<; fi

.PHONY: clean

clean:
	rm -f *.o $(OBJS)
