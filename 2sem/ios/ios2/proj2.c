#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include "customer.h"
#include "employee.h"
#include "proj2.h"

#define LOG_FILENAME "proj2.out"

void close_office(SharedState *state) {
    sem_wait(&state->office_sem);
    log_to_file(state, "closing\n");
    state->office_opened = false;
    sem_post(&state->office_sem);
}

// if arguments are invalid, returns config with .valid == false
Config parse_args(int argc, char **argv) {
    Config c;
    c.valid = true;

    if (argc != 6) {
        fprintf(stderr, "Usage: %s  NZ NU TZ TU F\n", argv[0]);
        c.valid = false;
        return c;
    }
    c.nz = atoi(argv[1]);
    c.nu = atoi(argv[2]);
    c.tz = atoi(argv[3]);
    c.tu = atoi(argv[4]);
    c.f = atoi(argv[5]);

    if (c.tz > 10000 || c.tu > 100 || c.f > 10000) {
        fprintf(stderr, "Invalid input parameter ranges.\n");
        c.valid = false;
    }
    if (c.nu == 0) {
        fprintf(stderr, "Number of emplyees cannot be zero.\n");
        c.valid = false;
    }

    return c;
}

int main(int argc, char **argv) {
    // this flag is used for determining return code
    bool success = true;

    // allocating and attaching shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedState), 0600 | IPC_CREAT);
    if (shmid == -1) {
        fprintf(stderr, "Error while allocating shared memory.\n");
        return EXIT_FAILURE;
    }
    SharedState *state = shmat(shmid, NULL, 0);
    if (state == (void *)-1) {
        fprintf(stderr, "Error while attaching shared memory.\n");
        success = false;
        goto ERR_FREE_SHM;
    }

    // setting default values of some shared variables
    state->office_opened = true;
    state->message_counter = 1;
    state->config = parse_args(argc, argv);
    if (!state->config.valid) {
        success = false;
        goto ERR_DETACH_SHM;
    }

    // initializing semaphores in `state->queues` to _zero_ - we will use them
    // to represent queues - customers will block when calling
    // `sem_wait` on `state->queue[i]`, until an employee "serves a request" by
    // calling `sem_post` on the same queue
    for (unsigned i = 0; i < 3; i++) {
        if (sem_init(&state->queues[i], 1, 0) != 0)
            success = false;
        // these counters represent the number of customers waiting in each
        // queue, we need this to differentiate between empty an nonempty queue
        state->queue_counters[i] = 0;
    }

    // guards `state->office_opened`
    int sem1 = sem_init(&state->office_sem, 1, 1);
    // guards the whole `state->queue_counters` array
    int sem2 = sem_init(&state->queue_sem, 1, 1);
    // guards `state->file` and `state->message_counter`
    // (they are used at the same time, we don't need two semaphores)
    int sem3 = sem_init(&state->logging_sem, 1, 1);

    if (sem1 != 0 || sem2 != 0 || sem3 != 0 || !success) {
        fprintf(stderr, "Error while initializing semaphores.\n");
        goto ERR_FREE_SEM;
    }

    // opening log file
    state->file = fopen(LOG_FILENAME, "w");
    if (state->file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", LOG_FILENAME);
        goto ERR_FREE_SEM;
    }

    // creating employee processes before customers - if one of the forks fails,
    // we can safely close the post office and wait for
    // their completion without causing a deadlock
    for (unsigned i = 0; i < state->config.nu; i++) {
        int pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error while forking process.\n");
            close_office(state);
            int _status;

            for (unsigned j = 0; j < i; j++)
                wait(&_status);

            success = false;
            goto ERR_CLOSE_FILE;
        } else if (pid == 0) {
            employee(i + 1, state);
        }
    }

    // creating customer processes
    for (unsigned i = 0; i < state->config.nz; i++) {
        int pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error while forking process.\n");
            close_office(state);
            int _status;

            for (unsigned j = 0; j < i + state->config.nu; j++)
                wait(&_status);

            success = false;
            goto ERR_CLOSE_FILE;
        } else if (pid == 0) {
            customer(i + 1, state);
        }
    }

    // everything happens during this sleep
    msleep(state->config.f / 2 + rand() % (state->config.f / 2 + 1));

    close_office(state);

    // we wait for all child processes to exit
    int status;
    for (unsigned i = 0; i < state->config.nz + state->config.nu; i++) {
        wait(&status);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            success = false;
        }
    }

    if (!success) {
        fprintf(stderr, "Some child processes did not exit successfully.\n");
        goto ERR_CLOSE_FILE;
    }

    // deallocating resources

ERR_CLOSE_FILE:
    fclose(state->file);

ERR_FREE_SEM:
    sem_destroy(&state->logging_sem);
    sem_destroy(&state->office_sem);
    sem_destroy(&state->queue_sem);

    for (unsigned i = 0; i < 3; i++)
        sem_destroy(&state->queues[i]);

ERR_DETACH_SHM:
    shmdt(state);
ERR_FREE_SHM:
    shmctl(shmid, IPC_RMID, NULL);

    if (success)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
