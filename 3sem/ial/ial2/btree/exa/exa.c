/*
 * Použití binárních vyhledávacích stromů.
 *
 * S využitím Vámi implementovaného binárního vyhledávacího stromu (soubory
 * ../iter/btree.c a ../rec/btree.c) implementujte funkci letter_count. Výstupní
 * strom může být značně degradovaný (až na úroveň lineárního seznamu) a tedy
 * implementujte i druhou funkci (bst_balance), která strom, na požadavek
 * uživatele, vybalancuje. Funkce jsou na sobě nezávislé a tedy automaticky
 * NEVOLEJTE bst_balance v letter_count.
 *
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Vypočítání frekvence výskytů znaků ve vstupním řetězci.
 *
 * Funkce inicilializuje strom a následně zjistí počet výskytů znaků a-z (case
 * insensitive), znaku mezery ' ', a ostatních znaků (ve stromu reprezentováno
 * znakem podtržítka '_'). Výstup je v uložen ve stromu.
 *
 * Například pro vstupní řetězec: "abBccc_ 123 *" bude strom po běhu funkce
 * obsahovat:
 *
 * key | value
 * 'a'     1
 * 'b'     2
 * 'c'     3
 * ' '     2
 * '_'     5
 *
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné
 * funkce.
 */
void letter_count(bst_node_t **tree, char *input) {
    bst_init(tree);

    for (unsigned i = 0; input[i] != '\0'; i++) {
        char next_char = input[i];

        // convert uppercase letters into lowercase letters
        if (next_char >= 'A' && next_char <= 'Z')
            next_char += 'a' - 'A';

        // convert all chars that are not a letter, nor a space, into underscore
        if (!(next_char >= 'a' && next_char <= 'z') && next_char != ' ')
            next_char = '_';

        int count;
        if (bst_search(*tree, next_char, &count))
            bst_insert(tree, next_char, count + 1);
        else
            bst_insert(tree, next_char, 1);
    }
}

/// recursively inserts all elements of node_array into tree, starting with the
/// middle element, and recursively calling this function on left and right part
/// of the rest of node_array
void insert_all_middle(bst_node_t **tree, bst_node_t **node_array,
                       unsigned start, unsigned end) {
    // if the portion of the list we're supposed to insert is empty, do nothing
    if (start == end)
        return;

    // insert middle element
    unsigned middle = (start + end) / 2;
    bst_node_t *middle_node = node_array[middle];
    bst_insert(tree, middle_node->key, middle_node->value);

    insert_all_middle(tree, node_array, start, middle);
    insert_all_middle(tree, node_array, middle + 1, end);
}

/**
 * Vyvážení stromu.
 *
 * Vyvážený binární vyhledávací strom je takový binární strom, kde hloubka
 * podstromů libovolného uzlu se od sebe liší maximálně o jedna.
 *
 * Předpokládejte, že strom je alespoň inicializován. K získání uzlů stromu
 * využijte vhodnou verzi vámi naimplmentovaného průchodu stromem. Následně
 * můžete například vytvořit nový strom, kde pořadím vkládaných prvků zajistíte
 * vyváženost.
 *
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné
 * funkce. Není nutné, aby funkce fungovala *in situ* (in-place).
 */
void bst_balance(bst_node_t **tree) {
    // initialize an empty array of nodes
    bst_items_t node_array;
    node_array.nodes = NULL;
    node_array.size = 0;
    node_array.capacity = 0;

    // sort pointers to nodes into array using in-order walk
    bst_inorder(*tree, &node_array);

    // initialize new, empty tree
    bst_node_t *balanced_tree;
    bst_init(&balanced_tree);

    // store nodes from array into new tree so that it's balanced
    insert_all_middle(&balanced_tree, node_array.nodes, 0, node_array.size);

    // delete original tree and replace pointer to it with ptr to balanced tree
    bst_dispose(tree);
    *tree = balanced_tree;

    free(node_array.nodes);
}
