#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "./list.c"

// Variable to check that has been entered into the thread.
int allDone = 0;

queue_t q;

// Thread to pop tasks off queue and process them
void *consumer(void *arg) {
    int num = (long) arg;           // thread number
    int returnFromDeque = 0;        // exit condition
    char *tmp = malloc(256);        // line to read
    long threadSum = 0;             // total words from thread

    // Note: if returnFromDeque = -1 then the queue was empty
    while (allDone == 0 || returnFromDeque != -1) {
        returnFromDeque = Dequeue(&q, &tmp);
        
        if (!returnFromDeque) {     // true when thread wasn't empty

            /* This code count the number of words.
             * If it sees white space, it sets an internal flag.
             * If it sees non-whitespace and the flag is set,
             * it increments the word count and resets the flag.
             */
            char currChar = tmp[0];
            int charNum = 0;
            int wordCnt = 0;
            int whiteSpaceFlag = 1;

            // check first character
            if (currChar != '\n' && currChar != 0 && currChar != ' ') {
                wordCnt = 1;
                whiteSpaceFlag = 0;
            }

            // check each following character
            while (currChar != 0 && currChar != '\n') {
                currChar = tmp[charNum];
                if (currChar == ' ' || currChar == '\n' || currChar == '\t'){
                    whiteSpaceFlag = 1;
                }
                else if (whiteSpaceFlag){
                    wordCnt++;
                    whiteSpaceFlag = 0;
                }
                charNum++;
            }

            // results from line
            printf("Task: %d, Count: %d, Line: %s", num, wordCnt, tmp);
            threadSum += wordCnt;
        }
    }

    free(tmp);
    return (void *) threadSum;
}

// Parent thread that reads and enqueues lines from stdin
void producer() {
    char input[256];
    char * fgot = fgets(input, 256, stdin);

    // Note: if the return from fgot was NULL then it read in EOF
    while (fgot != NULL){
        Enqueue(&q, input, fgot == NULL);
        fgot = fgets(input, 256, stdin);
    }
    allDone = 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect Usage: Please check README\n");
        exit(-1);
    }

    // read number of threads from arg
    int numThreads = atoi(argv[1]);
    if (numThreads <= 0){
        fprintf(stderr, "Incorrect Usage: Please check README\n");
        exit(-1);
    }
    pthread_t children[numThreads];

    Queue_Init(&q);

    // start consumer threads
    for (long i = 0; i < numThreads; i++) {
        assert(pthread_create(&children[i], NULL, consumer, (void *) i) == 0);
    }

    // producer reads from file and adds to queue
    producer();

    // accumulate each threads' totals
    int totalWords = 0;
    for (int i = numThreads - 1; i >= 0; i--) {
        long rValue;
        assert(pthread_join(children[i], (void **) &rValue) == 0);
        totalWords += (long) rValue;
    }

    // results
    printf("Program Finished. Total Word Count: %d\n", totalWords);
    return 0;
}
