#include "proj2.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// returns the index (0, 1, 2) of a random nonempty
// queue if there is one, otherwire returns -1
int pick_queue(SharedState *state) {
    unsigned rand_num = rand() % 3;
    if (state->queue_counters[rand_num] > 0)
        return rand_num;
    rand_num = (rand_num + 1) % 3;
    if (state->queue_counters[rand_num] > 0)
        return rand_num;
    rand_num = (rand_num + 1) % 3;
    if (state->queue_counters[rand_num] > 0)
        return rand_num;

    return -1;
}

// checks whether the queues are empty, serves a single request if not,
// takes break or goes home if yes, all this in loop
void employee(unsigned id, SharedState *state) {
    srand(time(NULL) * getpid());
    log_to_file(state, "U %u: started\n", id);
    while (true) {
        sem_wait(&state->office_sem);
        sem_wait(&state->queue_sem);

        int queue = pick_queue(state);

        // queues are empty
        if (queue < 0) {
            // office is open, but all queues are empty
            if (state->office_opened) {
                log_to_file(state, "U %u: taking break\n", id);
                sem_post(&state->queue_sem);
                sem_post(&state->office_sem);
                msleep(rand() % (state->config.tu + 1));
                log_to_file(state, "U %u: break finished\n", id);
                continue;
            }
            // office is closed and all queues are empty
            else {
                log_to_file(state, "U %u: going home\n", id);
                sem_post(&state->queue_sem);
                sem_post(&state->office_sem);
                break;
            }
        }

        // a queue is not empty
        else {
            sem_post(&state->office_sem);
            log_to_file(state, "U %u: serving a service of type %u\n", id,
                        queue + 1);

            state->queue_counters[queue]--;
            sem_post(&state->queues[queue]);

            sem_post(&state->queue_sem);

            usleep(rand() % 11);
            log_to_file(state, "U %u: service finished\n", id);
        }
    }

    exit(EXIT_SUCCESS);
}
