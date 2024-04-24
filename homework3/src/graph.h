#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

#include "NetlistParser.h"


// Node structure for each datapath component in the graph
class Node {
public:
    // Key attributes for distinguishing between different datapath components
    std::string name;
    std::string type;
    std::vector<std::string> inputs;
    std::string output;
    int64_t datawidth;
    int64_t latency;

    // Attributes for force scheduling algorithm
    int64_t latency_requirement;
    int64_t asap_time;
    int64_t alap_time;
    int64_t time_frame[2];
    int64_t fds_width;
    std::vector<double> fds_prob;
    int64_t fds_time;

    // Next and previous node arrays to keep track of dependencies between datapath components
    std::vector<Node*> next;
    std::vector<Node*> prev;

    // Constructors
    Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, int64_t latency_requirement, int64_t latency);
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
    std::vector<Node*> vertices;

    int64_t latency_requirement;

    // Graph constructor (uses input from netlist parser)
    Graph(std::vector<Operation> operations, int64_t latency_requirement);
    // Helpers for generating dependency between nodes and connecting with source and sink nodes
    void generate_components_and_dependencies(std::vector<Operation> operations);
    // << overload for print
    friend std::ostream& operator<<(std::ostream& os,  const Graph& graph);

};

#endif