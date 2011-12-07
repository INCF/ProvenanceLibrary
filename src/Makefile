CC=gcc -g
CFLAGS=-c -fPIC -I /usr/include/libxml2

all: testprov testneuroprov

library.dylib:
	$(CC) $(CFLAGS) provenance.c
	$(CC) -shared -lxml2 -o libprov.so provenance.o

neuroprovlibrary.dylib:
	$(CC) $(CFLAGS) provenance.c
	$(CC) $(CFLAGS) neuroprovenance.c
	$(CC) -shared -lxml2 -o libneuroprov.so neuroprovenance.o provenance.o

testprov: library.dylib
	$(CC) -L. -lprov testprov.c -o testprov

testneuroprov: neuroprovlibrary.dylib
	$(CC) -L. -lneuroprov testneuroprov.c -o testneuroprov

clean:
	rm -f *.o *.so testprov testneuroprov

