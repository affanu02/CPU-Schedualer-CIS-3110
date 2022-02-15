all: simcpu.o simcpu

simcpu.o: simcpu.c
	gcc simcpu.c -o simcpu.o

simcpu: simcpu.o
	gcc -Wall -pedantic -std=c99 simcpu.c -o simcpu

clean:
	rm simcpu simcpu.o