#define NR_TESTS 100

#include "btree.h"
#include "test_util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const int base_data_count = 15;
const char base_keys[] = {'H', 'D', 'L', 'B', 'F', 'J', 'N', 'A',
                          'C', 'E', 'G', 'I', 'K', 'M', 'O'};
const int base_values[] = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 16};

const int additional_data_count = 6;
const char additional_keys[] = {'S', 'R', 'Q', 'P', 'X', 'Y', 'Z'};
const int additional_values[] = {10, 10, 10, 10, 10, 10};

const int traversal_data_count = 5;
const char traversal_keys[] = {'D', 'B', 'A', 'C', 'E'};
const int traversal_values[] = {1, 2, 3, 4, 5};

void init_test() {
    printf("Binary Search Tree - testing script\n");
    printf("-----------------------------------\n");
    printf("\n");
}

TEST(test_tree_init, "Initialize the tree")
bst_init(&test_tree);
ENDTEST

TEST(test_tree_dispose_empty, "Dispose the tree")
bst_init(&test_tree);
bst_dispose(&test_tree);
ENDTEST

TEST(test_tree_search_empty, "Search in an empty tree (A)")
bst_init(&test_tree);
int result;
bst_search(test_tree, 'A', &result);
ENDTEST

TEST(test_tree_insert_root, "Insert an item (H,1)")
bst_init(&test_tree);
bst_insert(&test_tree, 'H', 1);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_search_root, "Search in a single node tree (H)")
bst_init(&test_tree);
bst_insert(&test_tree, 'H', 1);
int result;
bst_search(test_tree, 'H', &result);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_update_root, "Update a node in a single node tree (H,1)->(H,8)")
bst_init(&test_tree);
bst_insert(&test_tree, 'H', 1);
bst_print_tree(test_tree);
bst_insert(&test_tree, 'H', 8);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_insert_many, "Insert many values")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_search, "Search for an item deeper in the tree (A)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
int result;
bst_search(test_tree, 'A', &result);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_search_missing, "Search for a missing key (X)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
int result;
bst_search(test_tree, 'X', &result);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_delete_leaf, "Delete a leaf node (A)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_print_tree(test_tree);
bst_delete(&test_tree, 'A');
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_delete_left_subtree, "Delete a node with only left subtree (R)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_insert_many(&test_tree, additional_keys, additional_values,
                additional_data_count);
bst_print_tree(test_tree);
bst_delete(&test_tree, 'R');
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_delete_right_subtree,
     "Delete a node with only right subtree (X)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_insert_many(&test_tree, additional_keys, additional_values,
                additional_data_count);

bst_print_tree(test_tree);
bst_delete(&test_tree, 'X');
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_delete_both_subtrees, "Delete a node with both subtrees (L)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_insert_many(&test_tree, additional_keys, additional_values,
                additional_data_count);

bst_print_tree(test_tree);
bst_delete(&test_tree, 'L');
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_delete_missing, "Delete a node that doesn't exist (U)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_print_tree(test_tree);
bst_delete(&test_tree, 'U');
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_delete_root, "Delete the root node (H)")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_print_tree(test_tree);
bst_delete(&test_tree, 'H');
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_dispose_filled, "Dispose the whole tree")
bst_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_print_tree(test_tree);
bst_dispose(&test_tree);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_preorder, "Traverse the tree using preorder")
bst_init(&test_tree);
bst_insert_many(&test_tree, traversal_keys, traversal_values,
                traversal_data_count);
bst_preorder(test_tree, test_items);
bst_print_tree(test_tree);
bst_print_items(test_items);
ENDTEST

TEST(test_tree_inorder, "Traverse the tree using inorder")
bst_init(&test_tree);
bst_insert_many(&test_tree, traversal_keys, traversal_values,
                traversal_data_count);
bst_inorder(test_tree, test_items);
bst_print_tree(test_tree);
bst_print_items(test_items);
ENDTEST

TEST(test_tree_postorder, "Traverse the tree using postorder")
bst_init(&test_tree);
bst_insert_many(&test_tree, traversal_keys, traversal_values,
                traversal_data_count);
bst_postorder(test_tree, test_items);
bst_print_tree(test_tree);
bst_print_items(test_items);
ENDTEST

TEST(test_letter_count, "Count letters");
bst_init(&test_tree);
letter_count(&test_tree, "abBcCc_ 123 *");
bst_print_tree(test_tree);
ENDTEST

TEST(test_balance, "Count letters and balance");
bst_init(&test_tree);
letter_count(&test_tree, "abBcCc_ 123 *");
bst_balance(&test_tree);
bst_print_tree(test_tree);
ENDTEST

int check_balance(bst_node_t *tree) {
    if (tree == NULL) {
        return 0;
    }
    int left = check_balance(tree->left);
    int right = check_balance(tree->right);
    assert(abs(left - right) <= 1);
    int maxheight = left > right ? left : right;
    return maxheight + 1;
}

// prints all three passes over tree (preorder, inorder, postorder)
// and also the tree itself
void print_walks(bst_node_t *tree) {
    bst_print_tree(tree);

    bst_items_t *items = bst_init_items();

    bst_preorder(tree, items);
    bst_inorder(tree, items);
    bst_postorder(tree, items);

    bst_print_items(items);

    bst_reset_items(items);

    free(items);
}

// counts nodes in a tree
int count_nodes(bst_node_t *tree) {
    if (tree == NULL) {
        return 0;
    }

    return count_nodes(tree->left) + count_nodes(tree->right) + 1;
}

// checks whether the BST is really S (keys must be ordered)
int check_order(bst_node_t *tree) {
    if (tree == NULL) {
        return -1;
    }

    int left = check_order(tree->left);
    if (left != -1) {
        assert(left < tree->key);
    }

    int right = check_order(tree->right);
    if (right != -1) {
        assert(tree->key < right);
    }

    return tree->key;
}

TEST(test_chonker_tree,
     "test everything at once, in the most brutal way possible");

for (int test_nr = 0; test_nr < NR_TESTS; test_nr++) {
    bool keys[256] = {0};
    int values[256] = {0};
    bst_node_t *tree;
    bst_init(&tree);

    // insert some values
    int cycles = rand() % 500;
    for (int i = 0; i < cycles; i++) {
        unsigned char key = rand() % 256;
        int value = rand();

        keys[key] = true;
        values[key] = value;
        bst_insert(&tree, key, value);
    }

    check_order(tree);

    print_walks(tree);

    int nodes_before = count_nodes(tree);
    bst_balance(&tree);
    check_balance(tree);
    assert(nodes_before == count_nodes(tree));

    print_walks(tree);

    check_order(tree);

    // check those values
    for (int key = 0; key < 256; key++) {
        int value = -1;
        bool found = bst_search(tree, key, &value);

        if (keys[key]) {
            assert(found);
            assert(value == values[key]);
        } else {
            assert(!found);
            assert(value == -1);
        }
    }

    // delete some of those values
    for (int i = 0; i < cycles; i++) {
        unsigned char key = rand() % 256;

        keys[key] = false;
        bst_delete(&tree, key);
    }

    check_order(tree);

    print_walks(tree);

    nodes_before = count_nodes(tree);
    bst_balance(&tree);
    check_balance(tree);
    assert(nodes_before == count_nodes(tree));

    print_walks(tree);

    check_order(tree);

    // check the values again
    for (int key = 0; key < 256; key++) {
        int value = -1;
        bool found = bst_search(tree, key, &value);

        if (keys[key]) {
            assert(found);
            assert(value == values[key]);
        } else {
            assert(!found);
            assert(value == -1);
        }
    }

    bst_dispose(&tree);
}

bst_init(&test_tree);
ENDTEST

char convert_char(char c) {
    if ((c >= 'a' && c <= 'z') || (c == ' ')) {
        return c;
    }

    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    }

    return '_';
}

TEST(test_chonker_lettercount, "test lettercount function, extreme overkill");

for (int test_nr = 0; test_nr < NR_TESTS; test_nr++) {
    int input_length = rand() % 2000;
    unsigned char *input = malloc(input_length + 1);

    int results[256];
    for (int i = 0; i < 256; i++)
        results[i] = -1;

    for (int i = 0; i < input_length; i++) {
        char value = (rand() % 255) + 1;
        input[i] = value;
        int idx = convert_char(value);
        if (results[idx] == -1)
            results[idx] = 0;
        results[idx]++;
    }

    input[input_length] = '\0';

    bst_node_t *tree;
    bst_init(&tree);

    letter_count(&tree, (char *)input);

    for (int i = 0; i < 256; i++) {
        int value = -1;
        bst_search(tree, i, &value);
        assert(value == results[i]);
    }

    bst_dispose(&tree);
    free(input);
}

bst_init(&test_tree);
ENDTEST

int main() {
    srand(42);
    init_test();

    test_tree_init();
    test_tree_dispose_empty();
    test_tree_search_empty();
    test_tree_insert_root();
    test_tree_search_root();
    test_tree_update_root();
    test_tree_insert_many();
    test_tree_search();
    test_tree_search_missing();
    test_tree_delete_leaf();
    test_tree_delete_left_subtree();
    test_tree_delete_right_subtree();
    test_tree_delete_both_subtrees();
    test_tree_delete_missing();
    test_tree_delete_root();
    test_tree_dispose_filled();
    test_tree_preorder();
    test_tree_inorder();
    test_tree_postorder();
    test_letter_count();
    test_balance();
    test_chonker_tree();
    test_chonker_lettercount();
}
