#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

#include "NetlistParser.h"

// WRITE DESTRUCTORS!!!!!!!

enum Color{
    White,
    Gray,
    Black,
    Unset
};

enum Latency_type{
    Reg,
    Not_Reg,
};

// Node structure for each datapath component in the graph
class Node {
public:
    // Key attributes for distinguishing between different datapath components
    std::string name;
    std::string type;
    std::vector<std::string> inputs;
    std::string output;
    int64_t datawidth;

    // Additional attributes to make latency algorithm work
    Color color;
    Latency_type latency_type;
    float distance;
    float init_distance;
    int64_t last_reg;

    // Next and previous node arrays to keep track of dependencies between datapath components
    std::vector<Node*> next;
    std::vector<Node*> prev;

    // Constructors
    Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth);
    Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, Latency_type latency_type);
    Node(std::string name);
    // Setters for dependencies
    void setNext(Node* node);
    void setPrev(Node* node);
    // << overload for print
    friend std::ostream& operator<<(std::ostream& os,  const Node& node);
};

class Graph{
public:

    // Necessary nodes for the graph
    Node* source_node;
    Node* sink_node;
    std::vector<Node*> vertices;

    // Graph constructor (uses input from netlist parser)
    Graph(std::vector<Operation> operations);
    // Helpers for generating dependency between nodes and connecting with source and sink nodes
    void generate_components_and_dependencies(std::vector<Operation> operations);
    void resolve_dependencies();
    // Functions related to critical path calculation (taken from class notes (REG to REG delay) and expanded) 
    std::vector<Node*> topological_sort(std::vector<Node*> list, Node* vertex);
    std::vector<Node*> TS_visit(std::vector<Node*> list, Node* vertex);
    double longest_path();
    // << overload for print
    friend std::ostream& operator<<(std::ostream& os,  const Graph& graph);


};

#endif