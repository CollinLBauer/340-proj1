#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

// math.h is required for float modulus fmod(x,y)
// solves a bug in checking for page size being even multiple of mem size
// -lm flag is required as a compiler argument to import properly
#include <math.h>

struct job {

    int jobNum;
    int mem;
    struct job *next;
    int timeSlices;

};

struct queue {
    struct job *head;
    struct job *tail;

};

int main(int argc, char *argv[]) {
    // assert command line arguments
    if (argc != 8){
        printf("7 positional arguments required. Found %d.\n", argc-1);
        printf("Usage: ./jobSim <mem-size> <page-size> <num-jobs> <min-time> <max-time> <mind-mem> <max-mem>\n");
        exit(1);
    }

    // retrieve variables from command line arguments
    int memSize  = atoi(argv[1]);
    int pageSize = atoi(argv[2]);
    int numJobs  = atoi(argv[3]);
    int minTime  = atoi(argv[4]);
    int maxTime  = atoi(argv[5]);
    int minMem   = atoi(argv[6]);
    int maxMem   = atoi(argv[7]);

    // assert edge cases
    if (getenv("RANDOM_SEED") == NULL){
        printf("RANDOM_SEED environment variable has not been set, exiting.\n");
        exit(1);
    }
    if (fmod(memSize/(float)pageSize, 2) != 0) {
        printf("The memory size is not an even multiple of page size, exiting.\n");
        exit(1);
    }
    if (maxMem > memSize) {
        printf("Maximum memory size of job exceeds possible memory size, exiting.\n");
        exit(1);
    }
    if (minMem > maxMem) {
        printf("Minimum memory size of job is greater than maximum memory size, exiting.\n");
        exit(1);
    }
    if (minTime > maxTime) {
        printf("Minimum job time is greater than the maximum job time, exiting.\n");
        exit(1);
    }
    if (numJobs <= 0) {
        printf("Please enter a positive number of jobs\n");
        exit(1);
    }

    // retrieve random seed
    int seed = atoi(getenv("RANDOM_SEED"));
    srand(seed);
    printf("Seed: %d\n",seed); //debug
    //int seed = atoi(seedChar);

    struct job **pageTable = malloc(memSize/pageSize * sizeof(struct job*));
    struct queue *jobQueue = malloc(sizeof(struct queue));
    struct job *tempJob = NULL;

    printf("Size of pageTable: %ld\n", memSize/pageSize * sizeof(struct job*));
    printf("Size of single job: %ld\n", sizeof(struct job*));

    for (int i = 0; i < numJobs; i++) {
        tempJob = malloc(sizeof(struct job));
        tempJob->timeSlices = (rand() % (maxTime - minTime)) + minTime;
        tempJob->mem = (rand() % (maxMem - minMem)/(pageSize)) * pageSize + minMem;
        if (tempJob->mem % (2*pageSize) != 0 )
            tempJob->mem += pageSize;
        tempJob->jobNum = i;
        if (i == 0)
            jobQueue->head = tempJob;
        else
            jobQueue->tail->next = tempJob;
        jobQueue->tail = tempJob;
    }


    printf("Simulator Parameters:\n");
    printf("\tMemory Size: %d\n", memSize);
    printf("\tPage Size: %d\n", pageSize);
    printf("\tRandom Seed: %d\n", seed);
    printf("\tNumber of jobs: %d\n", numJobs);
    printf("\tRuntime (min-max) timesteps: %d-%d\n", minTime, maxTime);
    printf("\tMemory (min-max): %d-%d\n\n\n", minMem, maxMem);

    printf("Job Queue:\n");
    printf("\t Job #   Runtime   Memory\n");
    struct job *walker = jobQueue->head;
    for (int i = 0; i < numJobs; i++) {
        printf("\t     %d         %d        %d\n", walker->jobNum, walker->timeSlices, walker->mem);
        walker = walker->next;
    }

    printf("\nSimulator Starting:\n\n");

    


};

