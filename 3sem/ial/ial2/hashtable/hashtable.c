/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
    // not a great hash function, every permutation
    // if input characters will have the same hash
    int result = 1;
    int length = strlen(key);
    for (int i = 0; i < length; i++) {
        result += key[i];
    }
    return (result % HT_SIZE);
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
    for (int i = 0; i < HT_SIZE; i++) {
        (*table)[i] = NULL; // NULL in table field means the position is empty
    }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
    ht_item_t *item = (*table)[get_hash(key)];
    while (item != NULL) {
        if (strcmp(item->key, key)) { // if keys differ, go to next item
            item = item->next;
            continue;
        }

        return item;
    }

    return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
    ht_item_t *item = ht_search(table, key);

    if (item != NULL) { // item with this key already exists
        item->value = value;
        return;
    }

    int idx = get_hash(key);
    ht_item_t *old_first = (*table)[idx];

    // allocating new item and its key
    ht_item_t *new_first = malloc(sizeof(ht_item_t));
    char *key_copy = malloc(strlen(key) + 1); // +1 is for null byte
    if (new_first == NULL || key_copy == NULL)
        return;

    strcpy(key_copy, key);
    new_first->key = key_copy;
    new_first->value = value;
    new_first->next = old_first;

    (*table)[idx] = new_first;
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
    ht_item_t *item = ht_search(table, key);
    return item == NULL ? NULL : &item->value;
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
    int idx = get_hash(key);
    ht_item_t *first_item = (*table)[idx];

    // no items with such hash, do nothing
    if (first_item == NULL)
        return;

    // if first item matches, delete it and put next element into table
    if (!strcmp(first_item->key, key)) {
        ht_item_t *next = first_item->next;
        free(first_item->key);
        free(first_item);
        (*table)[idx] = next;
        return;
    }

    // otherwise, walk the list and stop on matched element
    ht_item_t *prev_item = first_item;
    ht_item_t *delete_item = first_item->next;
    while (delete_item != NULL) {
        // if delete_item matches, free it and replace field `next` on prev item
        if (!strcmp(delete_item->key, key)) {
            prev_item->next = delete_item->next;
            free(delete_item->key);
            free(delete_item);
            return;
        }

        // otherwise, advance both pointers
        prev_item = prev_item->next;
        delete_item = delete_item->next;
    }
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu
 * po inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
    for (int i = 0; i < HT_SIZE; i++) {
        ht_item_t *item = (*table)[i];
        while (item != NULL) {
            ht_item_t *tmp = item->next;
            free(item->key);
            free(item);
            item = tmp;
        }
        (*table)[i] = NULL;
    }
}
