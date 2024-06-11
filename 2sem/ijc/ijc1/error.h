// error.h
// Řešení IJC-DU1, příklad b), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#ifndef ERROR_MODULE
#define ERROR_MODULE

void warning(const char *fmt, ...);

void error_exit(const char *fmt, ...);

#endif // !ERROR_MODULE
