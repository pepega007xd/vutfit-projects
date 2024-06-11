/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h
 * a připravených koster funkcí implementujte binární vyhledávací
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree) { *tree = NULL; }

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    while (tree != NULL) {
        if (key < tree->key) // key is smaller, search in left subtree
            tree = tree->left;
        else if (key == tree->key) { // key matches, set value and return
            *value = tree->value;
            return true;
        } else // key is bigger, search in right subtree
            tree = tree->right;
    }
    // if while ended, key was not found
    return false;
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    while (*tree != NULL) {
        if (key < (*tree)->key) // key is smaller, search in left subtree
            tree = &((*tree)->left);
        else if (key == (*tree)->key) { // key matches, insert value and return
            (*tree)->value = value;
            return;
        } else // key is bigger, search in right subtree
            tree = &((*tree)->right);
    }
    // if while ended, key was not found -> we must create new node
    *tree = malloc(sizeof(bst_node_t));
    if (*tree == NULL)
        return;

    (*tree)->key = key;
    (*tree)->value = value;
    (*tree)->left = NULL;
    (*tree)->right = NULL;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    // first walk to rightmost subtree
    while ((*tree)->right != NULL)
        tree = &((*tree)->right);

    bst_node_t *to_delete = *tree;
    target->key = to_delete->key;
    target->value = to_delete->value;
    *tree = to_delete->left;
    free(to_delete);
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {
    while (*tree != NULL) {
        if (key < (*tree)->key) // key is smaller, search in left subtree
            tree = &((*tree)->left);
        else if (key == (*tree)->key) { // key matches, delete this node
            // node has two subtrees, use bst_replace_by_rightmost
            if ((*tree)->left != NULL && (*tree)->right != NULL) {
                bst_replace_by_rightmost(*tree, &((*tree)->left));
                return;
            }

            // this is either the non-empty subtree, or NULL if this node is a
            // leaf
            bst_node_t *replacement =
                (*tree)->left ? (*tree)->left : (*tree)->right;

            free(*tree);
            *tree = replacement;
        } else // key is bigger, search in right subtree
            tree = &((*tree)->right);
    }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {
    stack_bst_t s;
    stack_bst_t *stack = &s; // with this, i don't have to write & everywhere
    stack_bst_init(stack);
    stack_bst_push(stack, *tree); // push root node on top of stack

    // repeat until all nodes from stack are freed
    while (!stack_bst_empty(stack)) {
        bst_node_t *node = stack_bst_pop(stack);
        // free the leftmost path of subtrees, while pushing all right subtrees
        // onto stack
        while (node != NULL) {
            bst_node_t *tmp = node->left;
            stack_bst_push(stack, node->right);
            free(node);
            node = tmp;
        }
    }
    *tree = NULL;
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku
 * uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit,
                           bst_items_t *items) {
    while (tree != NULL) {
        bst_add_node_to_items(tree, items);
        stack_bst_push(to_visit, tree->right);
        tree = tree->left;
    }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {
    stack_bst_t s;
    stack_bst_t *stack = &s;
    stack_bst_init(stack);

    // walk the leftmost path from root, to get all right subtrees on the stack
    bst_leftmost_preorder(tree, stack, items);

    // process all the right subtrees in the same way
    while (!stack_bst_empty(stack)) {
        bst_node_t *node = stack_bst_pop(stack);
        bst_leftmost_preorder(node, stack, items);
    }
}

/*
 * Pomocná funkce pro iterativní inorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        tree = tree->left;
    }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {
    stack_bst_t s;
    stack_bst_t *stack = &s;
    stack_bst_init(stack);

    // push all left subtrees to stack
    bst_leftmost_inorder(tree, stack);

    // each node from stack is added to items, then its right subtree is pushed
    // on stack
    while (!stack_bst_empty(stack)) {
        bst_node_t *node = stack_bst_pop(stack);
        bst_add_node_to_items(node, items);
        bst_leftmost_inorder(node->right, stack);
    }
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez
 * použití vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        stack_bool_push(first_visit, true);
        tree = tree->left;
    }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {
    // stack for storing unprocessed nodes
    stack_bst_t s;
    stack_bst_t *stack = &s;
    stack_bst_init(stack);

    // stack for storing information about whether corresponding node's right
    // subtree was already processed
    stack_bool_t s2;
    stack_bool_t *first_visit = &s2;
    stack_bool_init(first_visit);

    // first put the leftmost nodes from root on stack
    bst_leftmost_postorder(tree, stack, first_visit);

    while (!stack_bst_empty(stack)) {
        // if node on top of stack is visited for the first time, process its
        // right subtree
        if (stack_bool_top(first_visit)) {
            stack_bool_pop(first_visit);
            stack_bool_push(first_visit, false);
            bst_node_t *node = stack_bst_top(stack);
            bst_leftmost_postorder(node->right, stack, first_visit);
        }
        // if its right subtree was already processed, process the node itself,
        // and pop it from stack
        else {
            stack_bool_pop(first_visit);
            bst_node_t *node = stack_bst_pop(stack);
            bst_add_node_to_items(node, items);
        }
    }
}
