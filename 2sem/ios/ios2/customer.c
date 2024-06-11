#include "proj2.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// checks whether the post office is open, if yes, enqueues itself,
// if not, goes home
void customer(unsigned id, SharedState *state) {
    srand(time(NULL) * getpid());

    log_to_file(state, "Z %u: started\n", id);
    msleep(rand() % (state->config.tz + 1));

    sem_wait(&state->office_sem);
    if (!state->office_opened) {
        log_to_file(state, "Z %u: going home\n", id);
        sem_post(&state->office_sem);
    } else {
        unsigned queue = rand() % 3;
        log_to_file(state, "Z %u: entering office for a service %u\n", id,
                    queue + 1);
        sem_post(&state->office_sem);

        modify_queue(state, queue, 1);

        // enqueue customer's request
        sem_wait(&state->queues[queue]);

        log_to_file(state, "Z %u: called by office worker\n", id);
        usleep(rand() % 11);
        log_to_file(state, "Z %u: going home\n", id);
    }

    exit(EXIT_SUCCESS);
}
