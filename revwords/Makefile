all: revwords

revwords: revwords.c
	gcc -std=c99 -L../lib -I../lib -o revwords revwords.c -lhelpers

clean:
	rm -f ./revwords