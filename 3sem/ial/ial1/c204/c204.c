/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

bool solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného
 * postfixového výrazu
 */
void untilLeftPar(Stack *stack, char *postfixExpression,
                  unsigned *postfixExpressionLength) {
    char symbol;

    while (!Stack_IsEmpty(stack)) {
        Stack_Top(stack, &symbol);
        Stack_Pop(stack);

        if (symbol == '(') {
            break;
        } else {
            postfixExpression[*postfixExpressionLength] = symbol;
            (*postfixExpressionLength)++;
        }
    }
}

// returns priority of given operator
// [+], [-] -> 0
// [*], [/] -> 1
int priority(char operator) {
    switch (operator) {
    case '+':
    case '-':
        return 0;
    case '*':
    case '/':
        return 1;
    default:
        fprintf(stderr,
                "Internal error: Incorrect character in function priority\n");
        return -1;
    }
}

// returns true iff given char is an operator
bool isOperator(char c) {
    switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
        return true;

    default:
        return false;
    }
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného
 * postfixového výrazu
 */
void doOperation(Stack *stack, char c, char *postfixExpression,
                 unsigned *postfixExpressionLength) {
    char stackTop;

    while (!Stack_IsEmpty(stack)) {
        Stack_Top(stack, &stackTop);

        // if operator on stack has higher or same priority as our operator,
        // append it to output
        if (isOperator(stackTop) && priority(stackTop) >= priority(c)) {
            Stack_Pop(stack);
            postfixExpression[*postfixExpressionLength] = stackTop;
            (*postfixExpressionLength)++;
        } else {
            break;
        }
    }

    Stack_Push(stack, c);
}

// returns true iff input char is variable or number
bool isTerm(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

// returns true iff input char is variable
bool isVariable(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte
 * namísto řetězce konstantu NULL.
 *
 * @param infixExpression vstupní znakový řetězec obsahující infixový výraz k
 * převedení
 *
 * @returns znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix(const char *infixExpression) {
    // allocate output string
    char *postfixExpression = malloc(MAX_LEN);
    if (postfixExpression == NULL) {
        return NULL;
    }

    // initialize operator stack
    Stack stack;
    Stack_Init(&stack);

    // process all chars from infixExpression
    unsigned infixIndex = 0;
    unsigned postfixIndex = 0;
    while (true) {
        char nextChar = infixExpression[infixIndex];
        infixIndex++;

        if (nextChar == '=') {
            // append remaining operators to output
            untilLeftPar(&stack, postfixExpression, &postfixIndex);
            postfixExpression[postfixIndex] = '=';
            postfixExpression[postfixIndex + 1] = '\0';
            break;
        } else if (isTerm(nextChar)) {
            postfixExpression[postfixIndex] = nextChar;
            postfixIndex++;
        } else if (isOperator(nextChar)) {
            doOperation(&stack, nextChar, postfixExpression, &postfixIndex);
        } else if (nextChar == '(') {
            Stack_Push(&stack, nextChar);
        } else if (nextChar == ')') {
            untilLeftPar(&stack, postfixExpression, &postfixIndex);
        } else {
            fprintf(stderr, "Invalid character in input: %c", nextChar);
        }
    }

    Stack_Dispose(&stack);
    return postfixExpression;
}

/**
 * Pomocná metoda pro vložení celočíselné hodnoty na zásobník.
 *
 * Použitá implementace zásobníku aktuálně umožňuje vkládání pouze
 * hodnot o velikosti jednoho byte (char). Využijte této metody
 * k rozdělení a postupné vložení celočíselné (čtyřbytové) hodnoty
 * na vrchol poskytnutého zásobníku.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value hodnota k vložení na zásobník
 */
void expr_value_push(Stack *stack, int value) {
    for (unsigned i = 0; i < sizeof(int); i++) {
        Stack_Push(stack, (char)value); // type cast cuts off higher bytes
        value >>= 8;
    }
}

/**
 * Pomocná metoda pro extrakci celočíselné hodnoty ze zásobníku.
 *
 * Využijte této metody k opětovnému načtení a složení celočíselné
 * hodnoty z aktuálního vrcholu poskytnutého zásobníku. Implementujte
 * tedy algoritmus opačný k algoritmu použitému v metodě
 * `expr_value_push`.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné celočíselné hodnoty z vrcholu zásobníku
 */
void expr_value_pop(Stack *stack, int *value) {
    for (unsigned i = 0; i < sizeof(int); i++) {
        char tmp;
        Stack_Top(stack, &tmp);
        Stack_Pop(stack);
        *value <<= 8;
        *value |= tmp;
    }
}

// finds the value of variable `varName`
int getVarValue(VariableValue variableValues[], char varName,
                int variableValueCount) {
    for (int i = 0; i < variableValueCount; i++) {
        if (variableValues[i].name == varName) {
            return variableValues[i].value;
        }
    }

    fprintf(stderr, "Unknown variable: %c", varName);
    return 0;
}

// evaluates single operator on last two values on stack,
// pushes result onto back onto the stack
// returns true iff the evaluation is successful
// (no division by zero)
bool evalOperator(Stack *stack, char operator) {
    // first argument on stack is right-hand side of operator,
    // eg. "(a-b)" is in postfix "ab-", where "ab" will be on stack,
    // therefore the first thing to pop will be "b", then "a"
    int lhs, rhs;
    expr_value_pop(stack, &rhs);
    expr_value_pop(stack, &lhs);

    switch (operator) {
    case '+':
        expr_value_push(stack, lhs + rhs);
        break;
    case '-':
        expr_value_push(stack, lhs - rhs);
        break;
    case '*':
        expr_value_push(stack, lhs * rhs);
        break;
    case '/':
        // check for division by zero
        if (rhs == 0) {
            return false;
        }
        expr_value_push(stack, lhs / rhs);
        break;
    default:
        fprintf(stderr, "Invalid operator: %c", operator);
    }

    return true;
}

/**
 * Tato metoda provede vyhodnocení výrazu zadaném v `infixExpression`,
 * kde hodnoty proměnných použitých v daném výrazu jsou definovány
 * v poli `variableValues`.
 *
 * K vyhodnocení vstupního výrazu využijte implementaci zásobníku
 * ze cvičení c202. Dále také využijte pomocných funkcí `expr_value_push`,
 * respektive `expr_value_pop`. Při řešení si můžete definovat libovolné
 * množství vlastních pomocných funkcí.
 *
 * Předpokládejte, že hodnoty budou vždy definovány
 * pro všechy proměnné použité ve vstupním výrazu.
 *
 * @param infixExpression vstpní infixový výraz s proměnnými
 * @param variableValues hodnoty proměnných ze vstupního výrazu
 * @param variableValueCount počet hodnot (unikátních proměnných
 *   ve vstupním výrazu)
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné hodnoty vyhodnocení vstupního výrazu
 *
 * @return výsledek vyhodnocení daného výrazu na základě poskytnutých hodnot
 * proměnných
 */
bool eval(const char *infixExpression, VariableValue variableValues[],
          int variableValueCount, int *value) {
    *value = 0;
    char *postfixExpression = infix2postfix(infixExpression);

    Stack stack;
    Stack_Init(&stack);

    // process all chars from postfixExpression
    unsigned index = 0;
    while (true) {
        char nextChar = postfixExpression[index];
        index++;

        if (isVariable(nextChar)) {
            expr_value_push(&stack, getVarValue(variableValues, nextChar,
                                                variableValueCount));
        } else if (nextChar >= '0' && nextChar <= '9') {
            expr_value_push(&stack, nextChar - '0');
        } else if (isOperator(nextChar)) {
            // fails if division by zero happens
            if (!evalOperator(&stack, nextChar)) {
                goto ERR_EXIT;
            }
        } else if (nextChar == '=') {
            expr_value_pop(&stack, value);
            break;
        } else {
            fprintf(stderr, "Invalid character in input: %c", nextChar);
        }
    }

    free(postfixExpression);
    Stack_Dispose(&stack);
    return true;

ERR_EXIT:
    free(postfixExpression);
    Stack_Dispose(&stack);
    return false;
}

/* Konec c204.c */
