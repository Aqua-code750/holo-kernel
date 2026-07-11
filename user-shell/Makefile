CC ?= gcc
CFLAGS ?= -O2 -Wall

all: holoshell

holoshell: holoshell.c
	$(CC) $(CFLAGS) -o $@ holoshell.c

clean:
	rm -f holoshell

.PHONY: all clean
