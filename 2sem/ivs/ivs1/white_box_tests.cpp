//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - test suite
//
// $NoKeywords: $ivs_project_1 $white_box_tests.cpp
// $Author:     JMENO PRIJMENI <xlogin00@stud.fit.vutbr.cz>
// $Date:       $2023-03-07
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author JMENO PRIJMENI
 *
 * @brief Implementace testu hasovaci tabulky.
 */

#include <cstdio>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"

#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy hasovaci tabulky, testujte nasledujici:
// 1. Verejne rozhrani hasovaci tabulky
//     - Vsechny funkce z white_box_code.h
//     - Chovani techto metod testuje pro prazdnou i neprazdnou tabulku.
// 2. Chovani tabulky v hranicnich pripadech
//     - Otestujte chovani pri kolizich ruznych klicu se stejnym hashem
//     - Otestujte chovani pri kolizich hashu namapovane na stejne misto v
//       indexu
//============================================================================//

class emptyTable : public ::testing::Test {
  protected:
    hash_map_t *map{hash_map_ctor()};
    int dummyint;
    hash_map_state_code_t retcode;

    void SetUp() override { map = hash_map_ctor(); }

    void TearDown() override { hash_map_dtor(map); }
};

class nonEmptyTable : public ::testing::Test {
  protected:
    hash_map_t *map;
    int dummyint;
    hash_map_state_code_t retcode;

    void SetUp() override {
        map = hash_map_ctor();
        hash_map_put(map, "kentus", 19090);
        hash_map_put(map, "blentus", 42);
        hash_map_put(map, "", -12345);
        hash_map_put(
            map, "name so long that it could potentially cause problems", 666);
    }

    void TearDown() override { hash_map_dtor(map); }
};

TEST(otherTests, ctorAndDtor) {
    auto newMap = hash_map_ctor();
    EXPECT_NE(newMap, nullptr);
    hash_map_dtor(newMap); // passes if this doesn't crash
}

TEST_F(emptyTable, clear) {
    hash_map_clear(map);
    EXPECT_EQ(map->used, 0);
    EXPECT_EQ(map->first, nullptr);
    EXPECT_EQ(map->last, nullptr);
}

TEST_F(nonEmptyTable, clear) {
    hash_map_clear(map);
    EXPECT_EQ(hash_map_get(map, "blentus", &dummyint), KEY_ERROR);
    EXPECT_EQ(map->used, 0);
    EXPECT_EQ(map->first, nullptr);
    EXPECT_EQ(map->last, nullptr);
}

TEST_F(emptyTable, reserve) {
    retcode = hash_map_reserve(map, 500);
    EXPECT_GE(map->allocated, 500);
    EXPECT_EQ(retcode, OK);
}

TEST_F(nonEmptyTable, reserve) {
    retcode = hash_map_reserve(map, 1);
    EXPECT_GE(map->allocated, 1);
    EXPECT_EQ(retcode, VALUE_ERROR);

    retcode = hash_map_reserve(map, 8);
    EXPECT_GE(map->allocated, 8);
    EXPECT_EQ(retcode, OK);
}

TEST_F(emptyTable, size) { ASSERT_EQ(hash_map_size(map), 0); }

TEST_F(nonEmptyTable, size) {
    EXPECT_EQ(hash_map_size(map), 4);

    hash_map_put(map, "novy1", 32);
    EXPECT_EQ(hash_map_size(map), 5);

    hash_map_remove(map, "novy1");
    EXPECT_EQ(hash_map_size(map), 4);
}

TEST_F(emptyTable, capacity) { ASSERT_EQ(hash_map_capacity(map), 8); }

TEST_F(nonEmptyTable, capacity) {
    // current size (4) is less than 2/3 of the initial capacity (8)
    EXPECT_EQ(hash_map_capacity(map), 8);

    hash_map_put(map, "novy1", -13446);
    hash_map_put(map, "novy2", -13446);
    EXPECT_EQ(hash_map_capacity(map), 6);
    // now the size (6) is greater than 2/3 of the original capacity (8)
    // therefore it should've been reallocated to 16
    EXPECT_EQ(hash_map_capacity(map), 16);
}

TEST_F(emptyTable, contains) {
    ASSERT_FALSE(hash_map_contains(map, "cokoliv"));
}

TEST_F(nonEmptyTable, contains) {
    ASSERT_TRUE(hash_map_contains(map, "blentus"));
    ASSERT_FALSE(hash_map_contains(map, "amogus"));
}
TEST_F(emptyTable, put) {
    retcode = hash_map_put(map, "kentus", 666);
    EXPECT_EQ(retcode, OK);
    hash_map_get(map, "kentus", &dummyint);
    EXPECT_EQ(dummyint, 666);
}

TEST_F(nonEmptyTable, put) {
    retcode = hash_map_put(map, "novy", 666);
    EXPECT_EQ(retcode, OK);
    hash_map_get(map, "novy", &dummyint);
    EXPECT_EQ(dummyint, 666);

    retcode = hash_map_put(map, "", 555);
    EXPECT_EQ(retcode, KEY_ALREADY_EXISTS);
    hash_map_get(map, "", &dummyint);
    EXPECT_EQ(dummyint, 555);

    retcode = hash_map_put(map, "blentus", 12345);
    EXPECT_EQ(retcode, KEY_ALREADY_EXISTS);
    hash_map_get(map, "blentus", &dummyint);
    EXPECT_EQ(dummyint, 12345);
}

TEST_F(emptyTable, get) {
    retcode = hash_map_get(map, "kentus", &dummyint);
    EXPECT_EQ(retcode, KEY_ERROR);
}

TEST_F(nonEmptyTable, get) {
    retcode = hash_map_get(map, "", &dummyint);
    EXPECT_EQ(retcode, OK);
    EXPECT_EQ(dummyint, -12345);

    retcode = hash_map_get(map, "kentus", &dummyint);
    EXPECT_EQ(retcode, OK);
    EXPECT_EQ(dummyint, 19090);

    retcode = hash_map_get(
        map, "name so long that it could potentially cause problems",
        &dummyint);
    EXPECT_EQ(retcode, OK);
    EXPECT_EQ(dummyint, 666);

    retcode = hash_map_get(map, "neexistuje", &dummyint);
    EXPECT_EQ(retcode, KEY_ERROR);
}

TEST_F(emptyTable, pop) {
    retcode = hash_map_pop(map, "neexistujici", &dummyint);
    ASSERT_EQ(retcode, KEY_ERROR);
}

TEST_F(nonEmptyTable, pop) {
    retcode = hash_map_pop(map, "neexistujici", &dummyint);
    EXPECT_EQ(retcode, KEY_ERROR);

    retcode = hash_map_pop(map, "blentus", &dummyint);
    EXPECT_EQ(retcode, OK);
    EXPECT_EQ(dummyint, 42);

    retcode = hash_map_pop(map, "blentus", &dummyint);
    EXPECT_EQ(retcode, KEY_ERROR);
}
TEST_F(emptyTable, remove) {
    retcode = hash_map_remove(map, "blablabla");
    ASSERT_EQ(retcode, KEY_ERROR);
}

TEST_F(nonEmptyTable, remove) {
    retcode = hash_map_remove(map, "blablabla");
    ASSERT_EQ(retcode, KEY_ERROR);

    retcode = hash_map_remove(map, "blentus");
    ASSERT_EQ(retcode, OK);

    retcode = hash_map_remove(map, "blentus");
    ASSERT_EQ(retcode, KEY_ERROR);
}

// special cases tests

TEST_F(emptyTable, hash_collision) {
    hash_map_put(map, "ab", 31);

    // "ba" will have the same hash as "ab"
    retcode = hash_map_put(map, "ba", 99);
    EXPECT_EQ(retcode, OK);

    hash_map_get(map, "ab", &dummyint);
    EXPECT_EQ(dummyint, 31);
    hash_map_get(map, "ba", &dummyint);
    EXPECT_EQ(dummyint, 99);

    // checks that this is actually a hash collision
    EXPECT_NE(map->first->next, nullptr);
}

TEST_F(emptyTable, index_collision) {
    map->allocated = 2;
    hash_map_put(map, "ab", 15);

    retcode = hash_map_put(map, "ad", -20);
    EXPECT_EQ(retcode, OK);

    hash_map_get(map, "ab", &dummyint);
    EXPECT_EQ(dummyint, 15);

    hash_map_get(map, "ad", &dummyint);
    EXPECT_EQ(dummyint, -20);

    // checks that this is actually an index collision
    EXPECT_NE(map->first->next, nullptr);
}

/*** Konec souboru white_box_tests.cpp ***/
