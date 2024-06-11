// ppm.c
// Řešení IJC-DU1, příklad b), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#include "ppm.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

struct ppm *ppm_read(const char *filename) {
    struct ppm *p = malloc(sizeof(struct ppm));
    if (p == NULL) {
        warning("Could not allocate memory\n");
        goto ERROR_DEALLOC_PPM;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        warning("Could not read file: %s\n", filename);
        goto ERROR_DEALLOC_PPM;
    }

    unsigned int width, height, colors;
    if (fscanf(file, "P6 %u %u %u ", &width, &height, &colors) != 3) {
        warning("Invalid PPM header in file: %s\n", filename);
        goto ERROR_DEALLOC_PPM;
    }

    if (colors != 255) {
        warning(

            filename, colors);
        goto ERROR_DEALLOC_PPM;
    }

    p->xsize = width;
    p->ysize = height;

    p->data = malloc(3 * width * height);

    for (unsigned long i = 0; i < 3 * width * height; i++) {
        int c = getc(file);
        if (c != EOF) {
            p->data[i] = (char)c;
        } else {
            warning("Incomplete image data in file: %s\n", filename);
            goto ERROR_DEALLOC_BITMAP;
        }
    }

    fclose(file);
    return p;

ERROR_DEALLOC_BITMAP:
    free(p->data);

ERROR_DEALLOC_PPM:
    free(p);

    return NULL;
}

int ppm_write(struct ppm *image, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return EXIT_FAILURE;
    }

    fprintf(file, "P6\n%u %u\n255\n", image->xsize, image->ysize);

    for (unsigned long i = 0; i < 3 * image->xsize * image->ysize; i++) {
        if (fputc(image->data[i], file) == EOF) {
            return EXIT_FAILURE;
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

void ppm_free(struct ppm *p) {
    free(p->data);
    free(p);
}
