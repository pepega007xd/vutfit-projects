#ifndef PROJ2_H
#define PROJ2_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    bool valid;
    unsigned nz;
    unsigned nu;
    unsigned tz;
    unsigned tu;
    unsigned f;
} Config;

// putting all data to shared memory for
// convenience - no global variables needed
typedef struct {
    Config config;

    // for `office_opened`
    sem_t office_sem;
    bool office_opened;

    // for `queue_counters`
    sem_t queue_sem;
    sem_t queues[3];
    unsigned queue_counters[3];

    // for `file` and `message_counter`
    sem_t logging_sem;
    FILE *file;
    unsigned message_counter;
} SharedState;

// implemented in utils.c
int msleep(unsigned milliseconds);
void log_to_file(SharedState *state, const char *msg, ...);
void modify_queue(SharedState *state, unsigned queue, int change);

#endif // PROJ2_H
