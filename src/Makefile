CC=gcc -g
CFLAGS=-I /usr/include/libxml2
LDFLAGS=-L. -lprov

all: testprov

library.dylib:
	$(CC) -c -fPIC $(CFLAGS) provenance.c
	$(CC) -shared -lxml2 -o libprov.so provenance.o

testprov: library.dylib
	$(CC) ${LDFLAGS} testprov.c -o testprov

clean:
	rm -f *.o *.so testprov
