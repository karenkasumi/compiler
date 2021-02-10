CFLAGS=-std=c11 -g -static

C: C.c

test: C 
		./test.sh

clean:
		rm -f C *.o *~ compile*

.PHONY: test clean