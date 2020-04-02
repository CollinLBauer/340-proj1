#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <semaphore.h>

typedef struct __node_t {
    char* value;
    struct __node_t *next;
} node_t;

typedef struct __queue_t {
    node_t *head;
    node_t *tail;
    sem_t head_lock, tail_lock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    assert(sem_init(&q->head_lock, 0, 1) == 0);
    assert(sem_init(&q->tail_lock, 0, 1) == 0);
}


void Enqueue(queue_t *q, char* value, int done) {
    
    if (done != 1) {
        node_t *tmp = malloc(sizeof(node_t));
        assert(tmp != NULL);
        tmp->value = malloc(256);
        strcpy(tmp->value, value);
        tmp->next = NULL;
        assert(sem_wait(&q->tail_lock) == 0);
        q->tail->next = tmp;
        q->tail = tmp;
        assert(sem_post(&q->tail_lock) == 0);
    }
    
    
}

int Dequeue(queue_t *q, char **value) {
    assert(sem_wait(&q->head_lock) == 0);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head == NULL) {
        assert(sem_post(&q->head_lock) == 0);
        return -1; //Queue was empty
    }
    strcpy(*value,new_head->value);
    q->head = new_head;
    assert(sem_post(&q->head_lock) == 0);
    free(tmp->value);
    free(tmp);
    return 0;
}

