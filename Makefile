CC=cc
CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

scc: $(OBJS)
	$(CC) -o scc $(OBJS)

$(OBJS): scc.h

test: scc
	./scc -test
	./test.sh

clean:
	rm -f scc *.o *~ tmp*
