
CFLAGS=-O2 -W -Wall

cobs_test: cobs_test.o cobs.o
	$(CC) -o $@ cobs_test.o cobs.o

test: cobs_test
	./cobs_test

clean:
	rm -f cobs_test *.o
