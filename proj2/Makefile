all: src

clean:
	rm jobSim

clb: src/jobSim-clb.c
	gcc -Wall -g -o jobSim src/jobSim-clb.c -lm

src: src/jobSim.c
	gcc -Wall -g -o jobSim src/jobSim.c -lm
