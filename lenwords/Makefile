all: lenwords

lenwords: lenwords.c
	gcc -std=c99 -L../lib -I../lib -o lenwords lenwords.c -lhelpers

clean:
	rm -f ./lenwords