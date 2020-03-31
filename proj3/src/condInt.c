#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#define MAX 256

int /*char* */buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;
int allDone = 0;

void put (int str /*char *str*/) {
    buffer[fill_ptr] = str;
    fill_ptr = (fill_ptr + 1) % MAX;
    count++;
}

int /*char**/ get() {
    /*char* */int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    count--;
    return tmp;
}

pthread_cond_t empty, fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    char input[256];
    char * fgot = fgets(input, 256, stdin);
    while (fgot != NULL){
        assert(pthread_mutex_lock(&mutex) == 0);
        while (count == MAX && allDone == 0) {
            assert(pthread_cond_wait(&empty, &mutex) == 0);
        }
        put(atoi(input));
        char * fgot = fgets(input, 256, stdin);
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

void *consumer(void *arg) {
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
        /*char **/int tmp = get();
        assert(pthread_cond_signal(&empty) == 0);
        assert(pthread_mutex_unlock(&mutex) == 0);
        printf("%d\n", tmp);
    }

    return NULL;
}

int main(int argc, char* argv[]) {

    int numThreads = atoi(argv[1]);
    pthread_t children[numThreads];

    for (long i = 0; i < numThreads; i++) {
        assert(pthread_create(&children[i], NULL, consumer, (void *) i) == 0);
    }

    pthread_t p2;
    assert(pthread_create(&p2, NULL, producer, NULL) == 0);

    assert(pthread_join(p2, NULL) == 0);
    


    for (int i = numThreads - 1; i >= 0; i--) {

        assert(pthread_join(children[i], NULL) == 0);
    }


    return 0;
}