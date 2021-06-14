CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

icc: $(OBJS)
		$(CC) -o icc $(OBJS) $(LDFLAGS)

$(OBJS): icc.h

test: icc
		./test.sh

clean:
		rm -f icc *.o *~ tmp*

.PHONY: test clean