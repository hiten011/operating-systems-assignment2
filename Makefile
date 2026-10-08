CC = gcc
CFLAGS = -O3 -Wall -Wextra -Wpointer-arith -Wstrict-prototypes -std=gnu89 -MMD -MP -pthread
LDFLAGS = -pthread

.PHONY: all clean
all: test-mergesort

test-mergesort: test-mergesort.o mergesort.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include test-mergesort.d mergesort.d

clean:
	rm -f test-mergesort.o mergesort.o test-mergesort.d mergesort.d test-mergesort
