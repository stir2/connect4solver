CFLAGS = -Wall -std=c99

all: play test

play: play.o solve.o transposition_table.o
	gcc $(CFLAGS) -o play solve.o play.o transposition_table.o

test: test.o solve.o transposition_table.o
	gcc $(CFLAGS) -o test solve.o test.o transposition_table.o

transposition_table.o: transposition_table.c transposition_table.h solve.h
solve.o: solve.c solve.h transposition_table.h
play.o: play.c solve.h transposition_table.h
test.o: test.c solve.h transposition_table.h
 
clean:
	rm solve play test *.o