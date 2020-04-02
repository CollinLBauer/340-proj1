#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#define MAX 256         // acts as max number of lines and max length of single line

char* buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;
int allDone = 0;

void put (char* str) {
    buffer[fill_ptr] = str;
    fill_ptr = (fill_ptr + 1) % MAX;
    count++;
}

char* get() {
    char* tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    count--;
    return tmp;
}

pthread_cond_t empty, fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* producer() {
    char input[MAX];
    char* fgot = fgets(input, MAX, stdin);         // first line of file
    while (fgot != NULL){
        assert(pthread_mutex_lock(&mutex) == 0);
        while (count == MAX && allDone == 0) {
            assert(pthread_cond_wait(&empty, &mutex) == 0);
        }
        put(input);
        char* fgot = fgets(input, MAX, stdin);     // iterates through rest of file
        if (fgot == NULL) {
            allDone = 1;
            assert(pthread_cond_broadcast(&fill) == 0);
            assert(pthread_mutex_unlock(&mutex) == 0); 
            break;
        }
        assert(pthread_cond_signal(&fill) == 0);
        assert(pthread_mutex_unlock(&mutex) == 0); 
    }
    return NULL;
}

void* consumer(void *arg) {
    int num = (long) arg;
    while (!allDone || count != 0) {

        assert(pthread_mutex_lock(&mutex) == 0);
        while (count == 0 && !allDone) {
            assert(pthread_cond_wait(&fill, &mutex) == 0);
        }
        if (allDone && count == 0) {
            assert(pthread_mutex_unlock(&mutex) == 0);
            break;
        }
        char *tmp = get();

        char currChar = -1;
        int charNum = 0;
        int wordCnt = 1;        // this assumes there will never be an empty line
        while (currChar != 0){
            currChar = tmp[charNum];
            if (currChar == ' '){
                wordCnt++;
            }
            charNum++;

        }
        printf("Thread %d: %s", num, tmp);
        printf("  %d words.\n", wordCnt);
        

        assert(pthread_cond_signal(&empty) == 0);
        assert(pthread_mutex_unlock(&mutex) == 0);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Number of threads not specified.\n");
        exit(1);
    }
    int numThreads = atoi(argv[1]);
    if (numThreads <= 0) { 
        fprintf(stderr, "Number of threads must be a positive integer.\n");
        exit(1);
    }

    pthread_t children[numThreads];
    for (long i = 0; i < numThreads; i++) {
        assert(pthread_create(&children[i], NULL, consumer, (void *) i) == 0);
    }

    producer();


    for (int i = numThreads - 1; i >= 0; i--) {

        assert(pthread_join(children[i], NULL) == 0);
    }


    return 0;
}
