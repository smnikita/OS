all: libhelpers.so

libhelpers.so: 	helpers.o
	gcc -shared helpers.o -o libhelpers.so

helpers.o: helpers.c
	gcc -c -std=c99 -fPIC helpers.c -o helpers.o

clean:
	rm -f libhelpers.so
	rm -f helpers.o
