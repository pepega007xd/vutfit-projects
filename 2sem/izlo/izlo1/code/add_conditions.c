#include "cnf.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

//
// LOGIN: xbrabl04
//

// Tato funkce je prikladem pouziti funkci create_new_clause,
// add_literal_to_clause a add_clause_to_formula Vysvetleni, co dela, naleznete
// v zadani
void example_condition(CNF *formula, unsigned num_of_subjects,
                       unsigned num_of_semesters) {
    assert(formula != NULL);
    assert(num_of_subjects > 0);
    assert(num_of_semesters > 0);

    for (unsigned subject_i = 0; subject_i < num_of_subjects; ++subject_i) {
        for (unsigned semester_j = 0; semester_j < num_of_semesters;
             ++semester_j) {
            // vytvori novou klauzuli
            Clause *example_clause =
                create_new_clause(num_of_subjects, num_of_semesters);
            // vlozi do klauzule literal x_{i,j}
            add_literal_to_clause(example_clause, true, subject_i, semester_j);
            // vlozi do klauzule literal ~x_{i,j}
            add_literal_to_clause(example_clause, false, subject_i, semester_j);
            // prida klauzuli do formule
            add_clause_to_formula(example_clause, formula);
        }
    }
}

// Tato funkce by mela do formule pridat klauzule predstavujici podminku a)
// Predmety jsou reprezentovany cisly 0, 1, ..., num_of_subjects-1
// Semestry jsou reprezentovany cisly 0, 1, ..., num_of_semesters-1
void each_subject_enrolled_at_least_once(CNF *formula, unsigned num_of_subjects,
                                         unsigned num_of_semesters) {
    assert(formula != NULL);
    assert(num_of_subjects > 0);
    assert(num_of_semesters > 0);

    // ZDE PRIDAT KOD

    for (unsigned p = 0; p < num_of_subjects; p++) {
        Clause *subject_p =
            create_new_clause(num_of_subjects, num_of_semesters);
        for (unsigned s = 0; s < num_of_semesters; s++) {
            add_literal_to_clause(subject_p, true, p, s);
        }
        add_clause_to_formula(subject_p, formula);
    }
}

// Tato funkce by mela do formule pridat klauzule predstavujici podminku b)
// Predmety jsou reprezentovany cisly 0, 1, ..., num_of_subjects-1
// Semestry jsou reprezentovany cisly 0, 1, ..., num_of_semesters-1
void each_subject_enrolled_at_most_once(CNF *formula, unsigned num_of_subjects,
                                        unsigned num_of_semesters) {
    assert(formula != NULL);
    assert(num_of_subjects > 0);
    assert(num_of_semesters > 0);

    // ZDE PRIDAT KOD
    unsigned count = 0;

    for (unsigned p = 0; p < num_of_subjects; p++) {
        for (unsigned semester_first = 0; semester_first < num_of_semesters;
             semester_first++) {
            for (unsigned semester_second = semester_first + 1;
                 semester_second < num_of_semesters; semester_second++) {
                Clause *conflict =
                    create_new_clause(num_of_subjects, num_of_semesters);
                add_literal_to_clause(conflict, false, p, semester_first);
                add_literal_to_clause(conflict, false, p, semester_second);
                add_clause_to_formula(conflict, formula);
            }
        }
    }
    printf("c Number of clauses in at_most_once: %u\n", count);
}

// Tato funkce by mela do formule pridat klauzule predstavujici podminku c)
// Promenna prerequisities obsahuje pole s poctem prvku rovnym
// num_of_prerequisities Predmety jsou reprezentovany cisly 0, 1, ...,
// num_of_subjects-1 Semestry jsou reprezentovany cisly 0, 1, ...,
// num_of_semesters-1
void add_prerequisities_to_formula(CNF *formula, Prerequisity *prerequisities,
                                   unsigned num_of_prerequisities,
                                   unsigned num_of_subjects,
                                   unsigned num_of_semesters) {
    assert(formula != NULL);
    assert(prerequisities != NULL);
    assert(num_of_subjects > 0);
    assert(num_of_semesters > 0);

    for (unsigned i = 0; i < num_of_prerequisities; ++i) {
        // prerequisities[i].earlier_subject je predmet, ktery by si mel student
        // zapsat v nekterem semestru pred predmetem
        // prerequisities[i].later_subject

        // ZDE PRIDAT KOD

        unsigned first_p = prerequisities[i].earlier_subject;
        unsigned second_p = prerequisities[i].later_subject;

        for (unsigned first_s = 0; first_s < num_of_semesters; first_s++) {
            for (unsigned second_s = first_s; second_s < num_of_semesters;
                 second_s++) {
                Clause *violation =
                    create_new_clause(num_of_subjects, num_of_semesters);
                add_literal_to_clause(violation, false, first_p, second_s);
                add_literal_to_clause(violation, false, second_p, first_s);
                add_clause_to_formula(violation, formula);
            }
        }
    }
}
