CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

C: $(OBJS)
		$(CC) -o  C $(OBJS) $(LDFLAGS)

$(OBJS): C.h

test: C
		./test.sh

clean:
		rm -f C *.o *~ tmp*

.PHONY: test clean