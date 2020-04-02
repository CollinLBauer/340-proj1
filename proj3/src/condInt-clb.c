#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "./list.c"

// Variable to check that has been entered into the thread.
int allDone = 0;

queue_t q;

// Thread to pop tasks off queue and process them
void *consumer(void *arg) {
    int num = (long) arg;
    int returnFromDeque = 0;
    char *tmp = malloc(256);
    long threadSum = 0;

    // Note: if returnFromDeque = -1 then the queue was empty
    while (allDone == 0 || returnFromDeque != -1) {
        
        returnFromDeque = Dequeue(&q, &tmp);
        
        //True when thread wasn't empty
        if (returnFromDeque != -1) {
            // word counter
            char currChar = tmp[0];
            int charNum = 0;
            int wordCnt = 0;
            int whiteSpaceFlag = 1;
            if (currChar != '\n' && currChar != 0 && currChar != ' ') {
                wordCnt = 1;
                whiteSpaceFlag = 0;
            }
            while (currChar != 0) {
                currChar = tmp[charNum];
                if (currChar == ' '){
                    whiteSpaceFlag = 1;
                }
                if (currChar != ' ' && whiteSpaceFlag){
                    wordCnt++;
                    whiteSpaceFlag = 0;
                }
                charNum++;
            }
            printf("Task: %d, Count: %d, Line: %s", num, wordCnt, tmp);
            threadSum += wordCnt;
        }
    }

    free(tmp);

    return (void *) threadSum;
}

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
        printf("Incorrect Usage: Please check README\n");
        exit(-1);
    }

    int numThreads = atoi(argv[1]);
    pthread_t children[numThreads];

    Queue_Init(&q);

    for (long i = 0; i < numThreads; i++) {
        assert(pthread_create(&children[i], NULL, consumer, (void *) i) == 0);
    }

    producer();

    int totalWords = 0;

    for (int i = numThreads - 1; i >= 0; i--) {
        long rValue;
        assert(pthread_join(children[i], (void **) &rValue) == 0);
        totalWords += (long) rValue;
    }

    printf("Program Finished. Total Word Count: %d\n", totalWords);


    return 0;
}
