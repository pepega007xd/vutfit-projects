// ppm.h
// Řešení IJC-DU1, příklad b), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#ifndef PPM_H
#define PPM_H

struct ppm {
    unsigned xsize;
    unsigned ysize;
    char *data; // RGB bajty, celkem 3*xsize*ysize
};

struct ppm *ppm_read(const char *filename);

int ppm_write(struct ppm *image, const char *filename);

void ppm_free(struct ppm *p);

#endif // PPM_H
