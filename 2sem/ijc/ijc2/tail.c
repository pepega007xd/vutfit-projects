// Filename: tail.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28
//
// Compiler: gcc-10.2.1-6

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINES_COUNT 10
#define BUFFER_SIZE 4096

// struct representing given arguments,
// negative number in `lines` means invalid arguments
typedef struct {
    int lines;
    char *filename;
} Config;

// struct representing circular buffer of strings,
// with size `buf_size`
//
// when head == tail, the buffer is empty
typedef struct {
    unsigned size;
    char **buf_start;
    unsigned head;
    unsigned tail;
} Circ_buf;

// returns NULL on allocation failure
Circ_buf *cb_create(unsigned size) {
    // we need an extra cell to differenitate between
    // a full and an empty buffer
    size++;

    Circ_buf *cb = malloc(sizeof(Circ_buf));
    if (cb == NULL)
        return NULL;

    char **buf_start = calloc(size, sizeof(char *));
    if (buf_start == NULL)
        goto ERR_FREE_BUF;

    for (unsigned i = 0; i < size; i++) {
        buf_start[i] = malloc(BUFFER_SIZE);
        if (buf_start[i] == NULL)
            goto ERR_FREE_BUF_START;
    }

    cb->size = size;
    cb->buf_start = buf_start;
    cb->head = 0;
    cb->tail = 0;
    return cb;

ERR_FREE_BUF_START:
    for (unsigned i = 0; i < size; i++)
        free(buf_start);

ERR_FREE_BUF:
    free(cb);

    return NULL;
}

// accepts a null-terminated string
void cb_put(Circ_buf *cb, char *str) {
    strncpy(cb->buf_start[cb->head], str, BUFFER_SIZE);

    cb->head = (cb->head + 1) % cb->size;
    if (cb->head == cb->tail)
        cb->tail = (cb->tail + 1) % cb->size;
}

// returns NULL if the buffer is empty
char *cb_get(Circ_buf *cb) {
    if (cb->head == cb->tail)
        return NULL;

    char *result = cb->buf_start[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;

    return result;
}

void cb_free(Circ_buf *cb) {
    for (unsigned i = 0; i < cb->size; i++) {
        free(cb->buf_start[i]);
    }

    free(cb->buf_start);
    free(cb);
}

Config parse_input(int argc, char **argv) {
    Config config = {DEFAULT_LINES_COUNT, NULL};
    switch (argc) {
    case 1:
        break;

    case 2:
        config.filename = argv[1];
        break;

    case 3:
        if (strcmp(argv[1], "-n")) {
            config.lines = -1;
        } else {
            config.lines = atoi(argv[2]);
        }
        break;

    case 4:
        if (strcmp(argv[1], "-n")) {
            config.lines = -1;
        } else {
            config.lines = atoi(argv[2]);
            config.filename = argv[3];
        }
        break;

    default:
        config.lines = -1;
    }

    return config;
}

bool line_length_warning = false;
// removes trailing newline if present,
// displays error if line was too long
void strip_line(char *line) {
    for (unsigned i = 0; i < BUFFER_SIZE; i++) {
        if (line[i] == '\n') {
            line[i] = '\0';
            return;
        }
    }

    if (!line_length_warning) {
        fprintf(
            stderr,
            "ERROR: Maximum allowed line length is %u. Line will be cut off.\n",
            // BUFFERSIZE minus '\0'
            BUFFER_SIZE - 1);
        line_length_warning = true;
    }
}

int main(int argc, char **argv) {
    Config config = parse_input(argc, argv);
    // printf("lines: %u, file: %s", config.lines,
    //        (config.file == NULL) ? "<stdin>" : config.file);

    if (config.lines < 0) {
        fprintf(stderr, "Usage: %s: [-n lines] [filename]\n", argv[0]);
        return EXIT_FAILURE;
    } else if (config.lines == 0)
        return EXIT_SUCCESS;

    unsigned lines = config.lines;

    FILE *file;
    if (config.filename == NULL) {
        file = stdin;
    } else {
        file = fopen(config.filename, "r");
    }
    if (file == NULL) {
        fprintf(stderr, "ERROR: File '%s' cannot be opened.\n",
                config.filename);
        return EXIT_FAILURE;
    }

    Circ_buf *cb = cb_create(lines);
    if (cb == NULL)
        goto ERR_FCLOSE;

    char *line_buf = malloc(BUFFER_SIZE);
    if (line_buf == NULL)
        goto ERR_FREE_CB;

    while (fgets(line_buf, BUFFER_SIZE, file) != NULL) {
        strip_line(line_buf);
        cb_put(cb, line_buf);
    }

    for (unsigned i = 0; i < lines; i++) {
        char *line = cb_get(cb);
        if (line == NULL)
            break;
        printf("%s\n", line);
    }

    fclose(file);
    cb_free(cb);
    free(line_buf);
    return EXIT_SUCCESS;

ERR_FREE_CB:
    cb_free(cb);

ERR_FCLOSE:
    fclose(file);
    fprintf(stderr, "ERROR: Allocation error.\n");
    return EXIT_FAILURE;
}
