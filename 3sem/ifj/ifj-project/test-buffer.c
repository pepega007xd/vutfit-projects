/*
 * test-buffer.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "buffer.h"
#include "global_include.h"

void test_buffer_push() {
    Buffer b = buffer_create();
    assert(b.length == 0);
    assert(b.content[0] == 0);

    buffer_push(&b, 'a');
    buffer_push(&b, 'b');
    buffer_push(&b, 'c');
    buffer_push(&b, 'd');

    assert(b.length == 4);
    assert(strcmp(b.content, "abcd") == 0);
    buffer_destroy(&b);
}

void test_buffer_append() {
    Buffer b = buffer_create();

    assert(buffer_append(&b, "abcd") == 4);
    assert(b.length == 4);
    assert(strcmp(b.content, "abcd") == 0);

    buffer_append(&b, "e");
    assert(b.length == 5);
    assert(strcmp(b.content, "abcde") == 0);

    buffer_append(&b, "");
    assert(b.length == 5);
    assert(strcmp(b.content, "abcde") == 0);

    buffer_append(&b, " kentus");
    assert(b.length == 12);
    assert(strcmp(b.content, "abcde kentus") == 0);

    buffer_destroy(&b);
}

void test_buffer_clear() {
    Buffer b = buffer_create();

    buffer_append(&b, "abcd");
    buffer_clear(&b);
    assert(b.length == 0);
    assert(b.content[0] == '\0');
}

void test_buffer_destroy() {

    Buffer b = buffer_create();

    buffer_push(&b, '$');
    buffer_append(&b, "1000");
    buffer_push(&b, '.');
    buffer_clear(&b);

    buffer_destroy(&b);
    assert(b.length == 0);
    assert(b.content == NULL);
    assert(b.capacity == 0);
}

int main() {
    sgc_init();

    test_buffer_push();
    test_buffer_append();
    test_buffer_clear();
    test_buffer_destroy();

    sgc_free_all();
    sgc_destroy();
    return 0;
}
