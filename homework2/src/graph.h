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


class Node {
public:
    std::string name;
    std::string type;
    std::vector<std::string> inputs;
    std::string output;
    int64_t datawidth;

    Color color;
    Latency_type latency_type;
    float distance;
    float init_distance;

    std::vector<Node*> next;
    std::vector<Node*> prev;

    Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth);
    Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, Latency_type latency_type);
    Node(std::string name);
    void setNext(Node* node);
    void setPrev(Node* node);
    friend std::ostream& operator<<(std::ostream& os,  const Node& node);
};

class Graph{
public:

    Node* source_node;
    Node* sink_node;
    std::vector<Node*> vertices;

    Graph(std::vector<Component> components, std::vector<Operation> operations);
    void generate_components_and_dependencies(std::vector<Component> components, std::vector<Operation> operations);
    void resolve_dependencies();
    std::vector<Node*> topological_sort(std::vector<Node*> list, Node* vertex);
    std::vector<Node*> TS_visit(std::vector<Node*> list, Node* vertex);
    double longest_path();
    friend std::ostream& operator<<(std::ostream& os,  const Graph& graph);


};

#endif