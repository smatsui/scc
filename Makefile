CC=gcc
CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

scc: $(OBJS)
	$(CC) -o scc $(OBJS)

$(OBJS): scc.h

test: scc
	./test.sh

clean:
	rm -f scc *.o *~ tmp*
