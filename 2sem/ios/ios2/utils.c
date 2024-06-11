#include "proj2.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// convenience function to sleep given number of milliseconds
int msleep(unsigned ms) { return usleep(1000 * ms); }

// logs `message` to file at state->file, prepending the messsage
// number, uses state->logging_sem as guard for file access and
// counter access
void log_to_file(SharedState *state, const char *message, ...) {
    va_list args;
    va_start(args, message);

    sem_wait(&state->logging_sem);

    fprintf(state->file, "%u: ", state->message_counter);
    vfprintf(state->file, message, args);
    state->message_counter++;
    fflush(state->file);

    sem_post(&state->logging_sem);

    va_end(args);
}

// adds `change` to queue no. `queue` atomically
void modify_queue(SharedState *state, unsigned queue, int change) {
    sem_wait(&state->queue_sem);
    state->queue_counters[queue] += change;
    sem_post(&state->queue_sem);
}
