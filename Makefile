CFLASS=-std=c11 -g -static

icc: icc.c

test: icc
		./test.sh

clean:
		rm -f icc *.o *~ tmp*

.PHONY: test clean