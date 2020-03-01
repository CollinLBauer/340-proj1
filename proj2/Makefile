all: main

clean:
	rm jobSim
	rm jobSim-debug

debug: src/jobSim-debug.c
	gcc -Wall -g -o jobSim-debug src/jobSim-debug.c -lm

main: src/jobSim.c
	gcc -Wall -g -o jobSim src/jobSim.c -lm
