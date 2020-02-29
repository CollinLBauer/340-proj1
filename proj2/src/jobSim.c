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
    struct job *nextInQueue;
    int timeSlices;
    int startTime;
    int endTime;

};

struct memChunk {
    int jobNum;
    int lengthInPages;
    struct memChunk *next;
    struct memChunk *prev;
};

int remainingMem(struct memChunk *myChunkyMonkey) {
    int rtn = myChunkyMonkey->lengthInPages;
    while (myChunkyMonkey != NULL) {
        rtn += myChunkyMonkey->lengthInPages;
        myChunkyMonkey = myChunkyMonkey->next;
    }
    return rtn;
}


void freeChunk(struct memChunk *chunk) {
    struct memChunk *temp = NULL;
    if (chunk->prev != NULL) {
        if (chunk->prev->jobNum == 0) {
            temp = chunk->prev;
            chunk->lengthInPages = chunk->lengthInPages + chunk->prev->lengthInPages;
            chunk->prev = chunk->prev->prev;
            chunk->prev->next = chunk;
            free(temp);
        }
    }

    if (chunk->next != NULL) {
        if (chunk->next->jobNum == 0) {
            temp = chunk->next;
            chunk->lengthInPages = chunk->lengthInPages + chunk->next->lengthInPages;
            chunk->next = chunk->next->next;
            chunk->next->prev = chunk;
            free(temp);
        }
    }
}

void displayMemory(struct memChunk *chunk) {
    int i = 0;
    while (chunk != NULL) {
        for (int j = 0; j < chunk->lengthInPages; j++) {
            if (i % 16 == 0 && i != 0) 
                printf("\n");
            else if (i % 4 == 0 && i != 0)
                printf(" ");
            if (chunk->jobNum == 0)
                printf(".");
            else 
                printf("%d", chunk->jobNum);
        }
        chunk = chunk->next;
    }

}


int main(int argc, char *argv[]) {
    // assert command line arguments
    if (argc != 8){
        printf("7 positional arguments required. Found %d.\n", argc-1);
        printf("Usage: ./jobSim <mem-size> <page-size> <num-jobs> <min-time> <max-time> <min-mem> <max-mem>\n");
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

    // instantiate jobs
    struct job *allJobs = NULL;
    struct job *tempJob = NULL;
    struct job *currJob = NULL;

    // generate jobs
    for (int i = 1; i <= numJobs; i++) {
        tempJob = malloc(sizeof(struct job));
        // Ternary operators are used here
        // catches edge cases where max and min values are equal
        tempJob->timeSlices = (maxTime - minTime) ? (rand() % (maxTime - minTime)) + minTime : maxTime;
        tempJob->mem = (maxMem - minMem) ? (rand() % (maxMem - minMem)/(pageSize)) * pageSize + minMem : maxMem;
        if (tempJob->mem % (2*pageSize) != 0 )
            tempJob->mem += pageSize;
        tempJob->jobNum = i;
        if (i == 1)
            allJobs = tempJob;
        else
            currJob->next = tempJob;
        currJob = tempJob;
    }

    currJob = allJobs;

    printf("Simulator Parameters:\n");
    printf("\tMemory Size: %d\n", memSize);
    printf("\tPage Size: %d\n", pageSize);
    printf("\tRandom Seed: %d\n", seed);
    printf("\tNumber of jobs: %d\n", numJobs);
    printf("\tRuntime (min-max) timesteps: %d-%d\n", minTime, maxTime);
    printf("\tMemory (min-max): %d-%d\n\n\n", minMem, maxMem);

    printf("Job Queue:\n");
    printf("\t Job #   Runtime   Memory\n");
    for (int i = 0; i < numJobs; i++) {
        printf("\t     %d         %d        %d\n", currJob->jobNum, currJob->timeSlices, currJob->mem);
        currJob = currJob->next;
    }

    // Resetting currJob to the head
    currJob = allJobs;

    printf("\nSimulator Starting:\n\n");

    struct job *queuedJob = NULL;
    struct memChunk *memory = malloc(sizeof(struct memChunk));

    memory->jobNum = 0;
    memory->lengthInPages = memSize/pageSize;
    //struct memChunk *tempMem = memory;
    //struct memChunk *currChunk = memory;


    while (currJob != NULL) {
        if (currJob->mem <= remainingMem(memory)) {
            if (queuedJob == NULL) {
                queuedJob = currJob;
                tempJob = queuedJob;
            }
            else {
                tempJob->nextInQueue = currJob;
                tempJob = tempJob->nextInQueue;
            }

            if (memory->jobNum == 0) {
                
            }

        }

        currJob = currJob->next;

    }
    printf("\nDone.\n");
    exit(0);
};

