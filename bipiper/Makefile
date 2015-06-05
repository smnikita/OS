all: forking polling

forking: forking.c
	gcc -Wall -I../lib -L../lib -o forking forking.c -lbufio

polling: polling.c
	gcc -g -std=c99 -I../lib -L../lib -o polling polling.c -lhelpers -lbufio

clean:
	rm -f ./forking
	rm -f ./polling