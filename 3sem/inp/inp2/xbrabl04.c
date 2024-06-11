#include <assert.h>
#include <stdio.h>
#include <string.h>

void insertionSort(char *str) {
    for (int idx = 0; str[idx] != 0; idx++) {
        char value = str[idx];
        for (int i = idx; i > 0; i--) {
            if (str[i - 1] > value) {
                char tmp = str[i - 1];
                str[i - 1] = value;
                str[i] = tmp;
            } else {
                break;
            }
        }
    }
}

void insertionSort2(char *str) {
    for (char *idx = str; *idx != 0; idx++) {
        for (char *insert = idx; insert != str; insert--) {
            char *prev = insert - 1;
            if (*prev > *insert) {
                char tmp = *prev;
                *prev = *insert;
                *insert = tmp;
            } else {
                break;
            }
        }
    }
}

int main() {
    char a[] = "acbdfeg";
    char b[] = "cagdbef";
    char c[] = "fedagbc";
    char correct[] = "abcdefg";

    insertionSort2(a);
    insertionSort2(b);
    insertionSort2(c);
    puts(a);
    puts(b);
    puts(c);

    assert(strcmp(a, correct) == 0);
    assert(strcmp(b, correct) == 0);
    assert(strcmp(c, correct) == 0);
    return 0;
}
