all: simplesh

simplesh: simplesh.c
	gcc -std=c99 -L../lib -I../lib -o simplesh simplesh.c -lhelpers -lbufio

clean:	
	rm -f ./simplesh