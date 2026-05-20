CFLAGS = -Wall -std=c99

all: solve

solve:
	gcc $(CFLAGS) solve.c -o solve

clean:
	rm solve