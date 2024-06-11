//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     TOMAS BRABLEC <xbrabl04@stud.fit.vutbr.cz>
// $Date:       $2023-02-18
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author TOMAS BRABLEC
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"
#include "red_black_tree_lib.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//
  
TEST(EmptyTree, InsertNode_0) {
    BinaryTree tree {};
    ASSERT_EQ(tree.InsertNode(42).first, true);
}

TEST(EmptyTree, DeleteNode_0){
    BinaryTree tree {};
    ASSERT_EQ(tree.DeleteNode(-2), false);
}

TEST(EmptyTree, FindNode_0) {
    BinaryTree tree {};
    ASSERT_EQ(tree.FindNode(42), nullptr);
}

TEST(NonEmptyTree, InsertNode_0) {
    BinaryTree tree {};
    auto first_node = tree.InsertNode(-2);
    auto second_node = tree.InsertNode(-2);
    ASSERT_EQ(second_node.first, false);
    ASSERT_EQ(first_node.second, second_node.second);
}

TEST(NonEmptyTree, DeleteNode_0){
    BinaryTree tree {};
    tree.InsertNode(0);
    ASSERT_EQ(tree.DeleteNode(0), true);
}

TEST(NonEmptyTree, FindNode_0) {
    BinaryTree tree {};
    auto node = tree.InsertNode(0).second;
    ASSERT_EQ(tree.FindNode(0), node);
}

TEST(TreeAxioms, Axiom1_0) {
    BinaryTree tree {};
    for (int i = 0; i < 100; i++) {
        tree.InsertNode(i);
    }

    std::vector<Node_t *> leaf_nodes {};
    tree.GetLeafNodes(leaf_nodes);

    for (auto leaf: leaf_nodes) {
        ASSERT_EQ(leaf->color, BLACK);
    }
}

TEST(TreeAxioms, Axiom2_0) {
    BinaryTree tree {};
    for (int i = 0; i < 100; i++) {
        tree.InsertNode(i);
    }

    std::vector<Node_t *> nodes {};
    tree.GetAllNodes(nodes);
    for (auto node: nodes) {
        if (node->color == RED) {
            ASSERT_EQ(node->pLeft->color, BLACK);
            ASSERT_EQ(node->pRight->color, BLACK);
        }
    }
}

int black_nodes_above(Node_t *node) {
    int count = 0;

    while (node != nullptr) {
        if (node->color == BLACK) {
            count++;
        }
        node = node->pParent;
    }
    return count;
} 

TEST(TreeAxioms, Axiom3_0) {
    BinaryTree tree {};
    for (int i = 0; i < 100; i++) {
        tree.InsertNode(i);
    }

    std::vector<Node_t *> leaf_nodes {};
    tree.GetLeafNodes(leaf_nodes);
    int black_count = black_nodes_above(leaf_nodes[0]);

    for (auto leaf: leaf_nodes) {
        ASSERT_EQ(black_nodes_above(leaf), black_count);
    }
}

/*** Konec souboru black_box_tests.cpp ***/
