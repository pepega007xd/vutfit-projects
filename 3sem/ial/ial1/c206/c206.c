/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
* uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */

void DLL_Error(void) {
    printf("*ERROR* The program has performed an illegal operation.\n");
    error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list) {
    list->firstElement = NULL;
    list->lastElement = NULL;
    list->activeElement = NULL;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose(DLList *list) {
    DLLElementPtr elem = list->firstElement;

    // if list is already empty, do nothing
    if (elem == NULL)
        return;

    // free all elements
    while (elem != NULL) {
        DLLElementPtr nextElem = elem->nextElement;
        free(elem);
        elem = nextElem;
    }

    // restore pointers
    list->firstElement = NULL;
    list->lastElement = NULL;
    list->activeElement = NULL;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst(DLList *list, int data) {
    DLLElementPtr prevFirstElem = list->firstElement;

    // allocate new element
    DLLElementPtr newFirstElem = malloc(sizeof(struct DLLElement));
    if (newFirstElem == NULL) {
        DLL_Error();
        return;
    }

    // set pointer values
    newFirstElem->data = data;
    newFirstElem->previousElement = NULL;
    // prevFirstElem is either NULL or valid pointer
    newFirstElem->nextElement = prevFirstElem;
    list->firstElement = newFirstElem;

    if (prevFirstElem == NULL) {
        // if the list was empty
        list->lastElement = newFirstElem;
    } else {
        // if there was an element before
        prevFirstElem->previousElement = newFirstElem;
    }
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k
 * DLL_InsertFirst). V případě, že není dostatek paměti pro nový prvek při
 * operaci malloc, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast(DLList *list, int data) {
    DLLElementPtr prevLastElem = list->lastElement;

    // allocate new element
    DLLElementPtr newLastElem = malloc(sizeof(struct DLLElement));
    if (newLastElem == NULL) {
        DLL_Error();
        return;
    }

    // set pointer values
    newLastElem->data = data;
    newLastElem->nextElement = NULL;
    // prevLastElem is either NULL or valid pointer
    newLastElem->previousElement = prevLastElem;
    list->lastElement = newLastElem;

    if (prevLastElem == NULL) {
        // if the list was empty
        list->firstElement = newLastElem;
    } else {
        // if there was en element before
        prevLastElem->nextElement = newLastElem;
    }
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list) { list->activeElement = list->firstElement; }

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list) { list->activeElement = list->lastElement; }

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst(DLList *list, int *dataPtr) {
    // check if list is empty
    if (list->firstElement == NULL) {
        DLL_Error();
        return;
    }

    *dataPtr = list->firstElement->data;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu
 * list. Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast(DLList *list, int *dataPtr) {
    // check if list is empty
    if (list->lastElement == NULL) {
        DLL_Error();
        return;
    }

    *dataPtr = list->lastElement->data;
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst(DLList *list) {
    DLLElementPtr prevFirstElem = list->firstElement;

    // check if list is already empty
    if (list->firstElement == NULL) {
        return;
    }

    // if the first element was active, make the list inactive
    if (list->activeElement == prevFirstElem) {
        list->activeElement = NULL;
    }

    DLLElementPtr newFirstElem = prevFirstElem->nextElement;
    free(prevFirstElem);

    if (newFirstElem == NULL) {
        // the deleted element was the only element in list
        list->firstElement = NULL;
        list->lastElement = NULL;
    } else {
        // another element remains in list
        newFirstElem->previousElement = NULL;
        list->firstElement = newFirstElem;
    }
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast(DLList *list) {
    DLLElementPtr prevLastElem = list->lastElement;

    // check if the list is already empty
    if (prevLastElem == NULL) {
        return;
    }

    // if the last element was active, make the list inactive
    if (list->activeElement == prevLastElem) {
        list->activeElement = NULL;
    }

    DLLElementPtr newLastElem = prevLastElem->previousElement;
    free(prevLastElem);

    if (newLastElem == NULL) {
        // the deleted element was the only element in list
        list->firstElement = NULL;
        list->lastElement = NULL;
    } else {
        // another element remains in list
        newLastElem->nextElement = NULL;
        list->lastElement = newLastElem;
    }
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter(DLList *list) {
    DLLElementPtr activeElem = list->activeElement;

    // check if there is an element to delete
    if (activeElem == NULL || list->lastElement == activeElem) {
        return;
    }

    DLLElementPtr prevAfterElem = activeElem->nextElement;
    DLLElementPtr newAfterElem = prevAfterElem->nextElement;

    free(prevAfterElem);
    activeElem->nextElement = newAfterElem;

    if (newAfterElem == NULL) {
        // if the active element is now last, set the "last element" pointer
        list->lastElement = activeElem;
    } else {
        // otherwise, set backward pointer on `newAfterElem`
        newAfterElem->previousElement = activeElem;
    }
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore(DLList *list) {
    DLLElementPtr activeElem = list->activeElement;

    // check if there is an element to delete
    if (activeElem == NULL || list->firstElement == activeElem) {
        return;
    }

    DLLElementPtr prevBeforeElem = activeElem->previousElement;
    DLLElementPtr newBeforeElem = prevBeforeElem->previousElement;

    free(prevBeforeElem);
    activeElem->previousElement = newBeforeElem;

    if (newBeforeElem == NULL) {
        // if the active element is now first, set the "first element" pointer
        list->firstElement = activeElem;
    } else {
        // otherwise, set forward pointer on `newBeforeElem`
        newBeforeElem->nextElement = activeElem;
    }
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter(DLList *list, int data) {
    DLLElementPtr activeElem = list->activeElement;

    // check if the list is active
    if (activeElem == NULL) {
        return;
    }

    // allocate new element
    DLLElementPtr newAfterElem = malloc(sizeof(struct DLLElement));
    if (newAfterElem == NULL) {
        DLL_Error();
        return;
    }
    newAfterElem->data = data;

    DLLElementPtr prevAfterElem = activeElem->nextElement;

    // set pointer values
    activeElem->nextElement = newAfterElem;
    newAfterElem->previousElement = activeElem;
    newAfterElem->nextElement = prevAfterElem;

    if (prevAfterElem == NULL) {
        // if newly inserted element is the last, set the "last pointer"
        list->lastElement = newAfterElem;
    } else {
        // else, set pointers between new and previous "afterElem"
        prevAfterElem->previousElement = newAfterElem;
    }
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore(DLList *list, int data) {
    DLLElementPtr activeElem = list->activeElement;

    // check if the list is active
    if (activeElem == NULL) {
        return;
    }

    // allocate new element
    DLLElementPtr newBeforeElem = malloc(sizeof(struct DLLElement));
    if (newBeforeElem == NULL) {
        DLL_Error();
        return;
    }
    newBeforeElem->data = data;

    DLLElementPtr prevBeforeElem = activeElem->previousElement;

    // set pointer values
    activeElem->previousElement = newBeforeElem;
    newBeforeElem->nextElement = activeElem;
    newBeforeElem->previousElement = prevBeforeElem;

    if (prevBeforeElem == NULL) {
        // if newly inserted element is the first, set the "first pointer"
        list->firstElement = newBeforeElem;
    } else {
        // else, set pointers between new and previous "beforeElem"
        prevBeforeElem->nextElement = newBeforeElem;
    }
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue(DLList *list, int *dataPtr) {
    // check if list is active
    if (list->activeElement == NULL) {
        DLL_Error();
    } else {
        *dataPtr = list->activeElement->data;
    }
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue(DLList *list, int data) {
    // check if list is active
    if (list->activeElement != NULL) {
        list->activeElement->data = data;
    }
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list) {
    // check if list is active
    if (list->activeElement != NULL) {
        list->activeElement = list->activeElement->nextElement;
    }
}

/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous(DLList *list) {
    // check if list is active
    if (list->activeElement != NULL) {
        list->activeElement = list->activeElement->previousElement;
    }
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive(DLList *list) { return (list->activeElement != NULL); }

/* Konec c206.c */
