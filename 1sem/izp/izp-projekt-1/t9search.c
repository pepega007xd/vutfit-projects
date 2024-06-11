#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// maximalni delka kontaktu
#define MAX_DELKA 100

// struktura pro sdeleni informaci o argumentech programu
typedef struct {
    bool platny_vstup;
    char *vyhl_ret;
    bool hledat_prerusovane;
} argumenty_t;

void error(char *zprava) {
    // vytiskne chybovou zpravu do stderr

    fprintf(stderr, "%s\n", zprava);
}

bool zkontroluj_vyhl_ret(char *vyhl_ret) {
    // vrati true, pokud je vyhledavaci retezec platny

    for (int i = 0;; i++) {
        if (vyhl_ret[i] == '\0')
            return true;
        if (vyhl_ret[i] < '0' || vyhl_ret[i] > '9') {
            error(
                "Neplatny vstup - vyhledavaci retezec obsahuje neplatne znaky");
            return false;
        }
    }
}

argumenty_t zpracuj_arg(int argc, char **argv) {
    // zpracuje argumenty ze standardniho vstupu,
    // zkontroluje platnost vyhl. retezce

    argumenty_t argumenty = {true, "", false};

    switch (argc) {
    case 1:
        break;

    case 2:
        if (zkontroluj_vyhl_ret(argv[1]))
            argumenty.vyhl_ret = argv[1];
        else
            argumenty.platny_vstup = false;
        break;

    case 3:
        if (strcmp(argv[1], "-s") == 0) {
            argumenty.hledat_prerusovane = true;
            if (zkontroluj_vyhl_ret(argv[2]))
                argumenty.vyhl_ret = argv[2];
            else
                argumenty.platny_vstup = false;
        } else {
            error("Neplatny vstup - neplatny argument");
            argumenty.platny_vstup = false;
        }
        break;

    default:
        error("prilis mnoho argumentu");
    }
    return argumenty;
}

int nacti_retezec(char *str) {
    // nacte retezec ze stdin do str,
    // zkontroluje delku, odstrani znak '\n'
    // vrati 1 pokud je ve stdin pouze EOF,
    // vrati -1 pokud je retezec neplatny,
    // vrati 0 pokud se nacteni zdarilo

    char docasny[MAX_DELKA + 2];

    if (fgets(docasny, MAX_DELKA + 2, stdin) == NULL)
        return 1;

    if (docasny[0] == '\0' || docasny[0] == '\n') {
        error("Neplatny vstup - prazdny kontakt");
        return -1;
    }

    for (int i = 0; i < MAX_DELKA + 1; i++) {
        if (docasny[i] == '\n' || docasny[i] == '\0') {
            docasny[i] = '\0';
            strncpy(str, docasny, 101);
            return 0;
        }
    }

    error("Neplatny vstup - prilis dlouhy kontakt");
    return -1;
}

bool shoda_znaku(char vzor, char znak) {
    // vrati true, pokud vzor (cislo 0-9)
    // odpovida znaku (jakykoli ASCII znak)

    static const char *TABULKA_HODNOT[] = {
        "0+",      "1",       "2abcABC",   "3defDEF", "4ghiGHI",
        "5jklJKL", "6mnoMNO", "7pqrsPQRS", "8tuvTUV", "9wxyzWXYZ"};

    for (int i = 0;; i++) {
        if (TABULKA_HODNOT[vzor - '0'][i] == '\0')
            return false;
        if (TABULKA_HODNOT[vzor - '0'][i] == znak)
            return true;
    }
}

bool shoda_retezce(argumenty_t argumenty, char *retezec) {
    // vrati true, pokud retezec obsahuje znaky,
    // ktere odpovidaji vyhledavacimu retezci

    int pocet_moznych_zacatku = strlen(retezec) - strlen(argumenty.vyhl_ret);

    if (argumenty.hledat_prerusovane) {
        for (int i = 0; i <= pocet_moznych_zacatku; i++) {
            for (int znak_retezce = 0, vyhl_znak = 0;; znak_retezce++) {
                if (argumenty.vyhl_ret[vyhl_znak] == '\0')
                    return true;

                if (retezec[i + znak_retezce] == '\0')
                    break;

                if (shoda_znaku(argumenty.vyhl_ret[vyhl_znak],
                                retezec[i + znak_retezce]))
                    vyhl_znak++;
            }
        }
    } else {
        for (int zacatek = 0; zacatek <= pocet_moznych_zacatku; zacatek++) {
            for (int vyhl_znak = 0;; vyhl_znak++) {
                if (argumenty.vyhl_ret[vyhl_znak] == '\0')
                    return true;

                if (retezec[zacatek + vyhl_znak] == '\0')
                    break;

                if (!shoda_znaku(argumenty.vyhl_ret[vyhl_znak],
                                 retezec[zacatek + vyhl_znak]))
                    break;
            }
        }
    }

    return false;
}

int main(int argc, char **argv) {
    // zpracovani argumentu
    argumenty_t argumenty = zpracuj_arg(argc, argv);
    if (!argumenty.platny_vstup)
        return EXIT_FAILURE;

    // 101. znak je pro '\0'
    char jmeno[MAX_DELKA + 1], cislo[MAX_DELKA + 1];
    bool nalezeno = false;
    int vysledek;

    // smycka, kterou zpracovavame jednotlive kontakty ze vstupu
    while (true) {
        // nacteme jmeno
        if ((vysledek = nacti_retezec(jmeno)) == 1) {
            if (!nalezeno)
                printf("Not found\n");
            break;
        } else if (vysledek == -1)
            return EXIT_FAILURE;

        // nacteme tel. cislo
        if ((vysledek = nacti_retezec(cislo)) == 1) {
            error("neplatna data - chybi tel. cislo");
            return EXIT_FAILURE;
        } else if (vysledek == -1)
            return EXIT_FAILURE;

        // proverime, zda vyhledavaci retezec odpovida
        // nekteremu z podretezcu jmena nebo cisla
        if (shoda_retezce(argumenty, jmeno) ||
            shoda_retezce(argumenty, cislo)) {
            printf("%s, %s\n", jmeno, cislo);
            nalezeno = true;
        }
    }
    return EXIT_SUCCESS;
}
