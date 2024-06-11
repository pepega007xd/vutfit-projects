/*
 * test-token_table.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "global_include.h"
#include "token_table.h"

extern TokenTable token_table;
Token test_tokens[] = {
    {.token_type = tt_yolo},
    {.token_type = tt_newl},
    {.token_type = tt_plus},
    {.token_type = tt_EOF},
};

void mock_init_token_table() {
    // init_token_table(); // this won't work

    // i don't know how to mock functions defined in another module,
    // so i just inject my own testing tokens into the table
    token_table.tokens = test_tokens;
    token_table.size = 4;
    token_table.ptr_idx = 0;

    assert(token_peek()->token_type == tt_yolo);
}

int main(void) {
    sgc_init();

    mock_init_token_table();
    token_table_start();

    assert(token_peek()->token_type == tt_yolo);
    token_next();
    assert(token_peek()->token_type == tt_newl);
    token_next();
    assert(token_peek()->token_type == tt_plus);
    token_next();
    assert(token_peek()->token_type == tt_EOF);
    // this should go on forever
    token_next();
    assert(token_peek()->token_type == tt_EOF);
    token_next();
    assert(token_peek()->token_type == tt_EOF);

    token_prev();
    assert(token_peek()->token_type == tt_plus);
    token_prev();
    assert(token_peek()->token_type == tt_newl);
    token_prev();
    assert(token_peek()->token_type == tt_yolo);
    token_prev();
    assert(token_peek() == NULL);
    // this should go on forever
    token_prev();
    assert(token_peek() == NULL);
    token_prev();
    assert(token_peek() == NULL);

    token_table_start();
    assert(token_peek()->token_type == tt_yolo);

    return 0;
}
