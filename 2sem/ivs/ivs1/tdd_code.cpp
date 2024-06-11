//======== Copyright (c) 2023, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - graph
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     TOMAS BRABLEC <xbrabl04@stud.fit.vutbr.cz>
// $Date:       $2023-02-18
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Martin Dočekal
 * @author Karel Ondřej
 *
 * @brief Implementace metod tridy reprezentujici graf.
 */

#include "tdd_code.h"
#include <cstdarg>
#include <iostream>
#include <iterator>
#include <ostream>
#include <set>
#include <stdexcept>
#include <vector>
#include <algorithm>

Graph::Graph(){}

Graph::~Graph(){}

std::pair<size_t, bool> Graph::find_node(size_t node_id) {
    auto result = std::find(nodes_data.begin(), nodes_data.end(), Node{node_id, 0});
    if (result == nodes_data.end()) {
        return {0, false};
    }

    return {distance(nodes_data.begin(), result), true};
}

std::vector<Node*> Graph::nodes() {
    std::vector<Node*> nodes;
    
    for (auto &node: nodes_data) {
        nodes.push_back(&node);
    }
    return nodes;
}

std::vector<Edge> Graph::edges() const{
    std::vector<Edge> edges;

    for (auto this_id: edges_data) {
        for (auto other_id: this_id.second) {
            Edge edge {this_id.first, other_id};
            if (find(edges.begin(), edges.end(), edge) == edges.end()) {
                edges.push_back(edge);
            }
        }
    }

    return edges;
}

Node* Graph::addNode(size_t nodeId) {
    if (find_node(nodeId).second) {
        return nullptr;
    }

    nodes_data.push_back(Node {nodeId, 0});
    edges_data.insert({nodeId, std::set<size_t> {}});
    return &nodes_data[find_node(nodeId).first];
}

bool Graph::addEdge(const Edge& edge){
    if (edge.a == edge.b) {
        return false;
    }

    addNode(edge.a);
    addNode(edge.b);

    edges_data[edge.a].insert(edge.b);
    auto result = edges_data[edge.b].insert(edge.a);
    return result.second;
}

void Graph::addMultipleEdges(const std::vector<Edge>& edges) {
    for (auto edge: edges) {
        addEdge(edge);
    }
}

Node* Graph::getNode(size_t nodeId){
    if (find_node(nodeId).second) {
        return &nodes_data[find_node(nodeId).first];
    }
    return nullptr;
}

bool Graph::containsEdge(const Edge& edge) const{
    try {
        auto nodemap = edges_data.at(edge.a);
        if (nodemap.find(edge.b) == nodemap.end()) {
            return false;
        }
        return true;
    }
    catch(std::out_of_range) {
        return false;
    }
}

void Graph::removeEdge(const Edge& edge){
    edges_data.at(edge.a).erase(edge.b);
    if (edges_data.at(edge.b).erase(edge.a) == 0) {
        throw std::out_of_range("edge not found");
    }
}

void Graph::removeNode(size_t nodeId){
    for (auto node: edges_data.at(nodeId)) {
        edges_data.at(node).erase(nodeId);
    }
    edges_data.erase(nodeId);

    nodes_data.erase(nodes_data.begin()+find_node(nodeId).first);
}

size_t Graph::nodeCount() const{
    return nodes_data.size();
}

size_t Graph::edgeCount() const{
    size_t edge_count = 0;

    for (auto node: edges_data) {
        edge_count += node.second.size();
    }

    return edge_count / 2;
}

size_t Graph::nodeDegree(size_t nodeId) const{
    return edges_data.at(nodeId).size();
}

size_t Graph::graphDegree() const{
    size_t graph_degree = 0;
    for (auto node: edges_data) {
        if (node.second.size() > graph_degree) {
            graph_degree = node.second.size();
        }
    }
    return graph_degree;
}

void Graph::color_recursive(Node &node, std::set<size_t> &colors) {
    if (node.color != 0) {
        return;
    }

    std::set<size_t> neighbor_colors {0};
    for (auto neighbor_id: edges_data.at(node.id)) {
        neighbor_colors.insert(nodes_data[find_node(neighbor_id).first].color);
    }

    std::set<size_t> available_colors;
    std::set_difference(colors.begin(), colors.end(),
            neighbor_colors.begin(), neighbor_colors.end(),
            std::inserter(available_colors, available_colors.begin()));

    if (available_colors.empty()) {
        colors.insert((*colors.rbegin()) + 1);
        node.color = *colors.rbegin();
    } else {
        node.color = *available_colors.begin();
    }

    for (auto neighbor_id: edges_data.at(node.id)) {
        color_recursive(nodes_data[find_node(neighbor_id).first], colors);
    }
}

void Graph::coloring(){
    if (nodeCount() == 0) {
        return;
    }

    std::set<size_t> colors {0};
    
    for (auto &node: nodes_data) {
        color_recursive(node, colors);
    }

}

void Graph::clear() {
    nodes_data.clear();
    edges_data.clear();
}

/*** Konec souboru tdd_code.cpp ***/
