#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>


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
    std::string input1;
    std::string input2;
    std::string output;
    int64_t datawidth;

    Color color;
    Latency_type latency_type;
    float distance;
    float init_distance;

    std::vector<Node*> next;
    std::vector<Node*> prev;

    Node(std::string name, std::string type, std::string input1, std::string input2, std::string output, int64_t datawidth);
    Node(std::string name, std::string type, std::string input1, std::string input2, std::string output, int64_t datawidth, Latency_type latency_type);
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

    Graph(std::string file_name);
    void form_graph(std::string file_name);
    void resolve_dependencies();
    std::vector<Node*> topological_sort(std::vector<Node*> list, Node* vertex);
    std::vector<Node*> TS_visit(std::vector<Node*> list, Node* vertex);
    void longest_path();
    friend std::ostream& operator<<(std::ostream& os,  const Graph& graph);


};

#endif