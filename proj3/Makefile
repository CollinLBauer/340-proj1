all: main

clean:
	rm condInt

clb: src/condInt-clb.c
	gcc -Wall -g -pthread -o condInt src/condInt-clb.c

main: src/condInt.c
	gcc -Wall -g -pthread -o condInt src/condInt.c