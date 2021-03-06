#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <math.h>

enum jobState {READY, SCHEDULED, DEAD};

// Define the job struct
struct job {
    int jobNum;
    int mem;
    int nextInSchedule;
    int timeSlices;
    int startTime;
    int endTime;
    enum jobState state;
};

// Finds how many free pages are in memory
int remainingMem(int memory[], int memSize) {
    int rtn = 0;
    for (int i = 0; i < memSize; i++) {
        if (memory[i] == -1)
            rtn += 1;
    }
    return rtn;
}

// Prints out the page table
void displayPages(int memory[], int memSize) {
    printf("   Page table: \n      ");
    for (int i = 0; i < memSize; i++) {
        if (i % 16 == 0 && i != 0) 
            printf("\n      ");
        else if (i % 4 == 0 && i != 0) 
            printf(" ");

        if (memory[i] == -1)
            printf(". ");
        else
            printf("%d ", memory[i] + 1);
    }
    printf("\n\n");
}

// Inserts a job into the memory
// Precondition: there are enough spots in the memory for the job to be inserted in the page table
void insertJob(int memory[], struct job *theJob, int pageSize) {
    int filled = 0;
    int i = 0;
    printf("   Job %d starting\n", theJob->jobNum + 1);
    while (filled < theJob->mem/pageSize) {
        if (memory[i] == -1) {
            memory[i] = theJob->jobNum;
            filled++;
        }
        i++;
    }
}

// Adds a job to the end of the scheduler
// Should only be run if the job is in memory
void addToScheduler(struct job **allJobs, int index, int frontOfScheduler) {
    while (allJobs[frontOfScheduler]->nextInSchedule != -1 )
        frontOfScheduler = allJobs[frontOfScheduler]->nextInSchedule;

    allJobs[frontOfScheduler]->nextInSchedule = index;
    allJobs[index]->state = SCHEDULED;
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

    // instantiate jobs
    struct job **allJobs = malloc(numJobs * sizeof(struct job));
    if (allJobs == NULL) {
        printf("Malloc failed. Exiting.");
        exit(1);
    }
    for (int i = 0; i < numJobs; i++) {
        allJobs[i] = malloc(sizeof(struct job));
        if (allJobs == NULL) {
            printf("Malloc failed. Exiting.");
            exit(1);
        }
    }

    // generate jobs
    int totalTime = 0;
    for (int i = 0; i < numJobs; i++) {
        // Ternary operators catch edge cases where max and min values are equal
        allJobs[i]->timeSlices = (maxTime - minTime) ? (rand() % (maxTime - minTime)) + minTime : maxTime;
        allJobs[i]->mem = (maxMem - minMem) ? (rand() % (maxMem - minMem)/(pageSize)) * pageSize + minMem : maxMem;

        //if (allJobs[i]->mem % (2*pageSize) != 0 )
        //    allJobs[i]->mem += pageSize;

        allJobs[i]->jobNum = i;
        allJobs[i]->nextInSchedule = -1;
        allJobs[i]->state = READY;
        totalTime += allJobs[i]->timeSlices;
    }

    // list accepted paraeters
    printf("Simulator Parameters:\n");
    printf("\tRandom Seed: %d\n", seed);
    printf("\tMemory Size: %d\n", memSize);
    printf("\tPage Size: %d\n", pageSize);
    printf("\tNumber of jobs: %d\n", numJobs);
    printf("\tRuntime (min-max) timesteps: %d-%d\n", minTime, maxTime);
    printf("\tMemory (min-max): %d-%d\n\n\n", minMem, maxMem);

    // list generated jobs
    printf("Job Queue:\n");
    printf("\t Job #   Runtime   Memory\n");
    for (int i = 0; i < numJobs; i++) {
        printf("\t     %d         %d    %d\n", allJobs[i]->jobNum + 1, allJobs[i]->timeSlices, allJobs[i]->mem);
    }

    printf("\nSimulator Starting:\n\n");

    // Creates memory and sets all elements of it to be -1
    // -1 means that page is unused and i >= 0 means that that page is being used by job i + 1
    int memory[memSize/pageSize];
    memset(memory, -1, memSize/pageSize * sizeof(int));


    // We are having a linked list within all of the jobs in the queue to keep track of which job is running
    // schedulerFront denotes which job should run next. A value of -1 signifies that no jobs are currently
    // scheduled.
    int schedulerFront = -1;

    // We can make use of assumptions 2, 4, and 5 from 7.1 in the textbook
    // we know how long all the jobs should run in total so we can determine 
    // how long the whole simulation should run.
    for (int i = 0; i < totalTime; i++) {
        printf("Time Step: %d\n", i + 1);

        // Checks to see if any unscheduled jobs can fit in the free memory and enter the scheduler
        for (int j = 0; j < numJobs; j++) {
            if (allJobs[j]->mem/pageSize <= remainingMem(memory, memSize/pageSize) && allJobs[j]->state == READY) {
                if (schedulerFront == -1) {
                    schedulerFront = j;
                    allJobs[schedulerFront]->nextInSchedule = -1;
                }
                else
                    addToScheduler(allJobs, j, schedulerFront);

                insertJob(memory, allJobs[j], pageSize);
                allJobs[j]->startTime = i + 1;
            }
        }

        // Decrements the remaining time slices for the running job by 1
        allJobs[schedulerFront]->timeSlices -= 1;
        printf("   Job %d Running\n", schedulerFront + 1);

        // Checks to see if the current job is now done
        if (allJobs[schedulerFront]->timeSlices == 0) {
            printf("   Job %d Completed\n", schedulerFront + 1);
            allJobs[schedulerFront]->endTime = i + 1;

            for (int k = 0; k < memSize/pageSize; k++) {
                if (memory[k] == allJobs[schedulerFront]->jobNum) 
                    memory[k] = -1;
            }

            allJobs[schedulerFront]->state = DEAD;
            schedulerFront = allJobs[schedulerFront]->nextInSchedule;
        }

        // If there is another job in the schedule then move the next job to the front
        // and pushes the running job all the way to the back.
        if (schedulerFront >= 0 && allJobs[schedulerFront]->nextInSchedule != -1) {
                int temp = schedulerFront;
                schedulerFront = allJobs[schedulerFront]->nextInSchedule;
                allJobs[temp]->nextInSchedule = -1;
                addToScheduler(allJobs, temp, schedulerFront);
        }
        // prints the current state of the memory 
        displayPages(memory, memSize/pageSize);
    }

    // display final job information and free memory
    printf("Job Information:\n\n");
    printf("   Job #   Start Time   End Time\n");
    for (int i = 0; i < numJobs; i++) {
        printf("       %d            %d         %d\n", i + 1, allJobs[i]->startTime, allJobs[i]->endTime);
        free(allJobs[i]);
    }

    // finish execution
    free(allJobs);
    printf("\nDone.\n");
    exit(0);
};
