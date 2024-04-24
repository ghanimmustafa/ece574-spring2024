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

    // Attributes for force scheduling algorithm
    int64_t latency_requirement;
    int64_t depth;
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
    Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, int64_t latency_requirement);
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

// class FDS{
//     public:
//         Graph *graph;
//         // Might be handled better...
//         std::vector<double> add_sub_prob;
//         std::vector<double> mul_prob;
//         std::vector<double> log_prob;
//         std::vector<double> div_mod_prob;

//         int64_t latency_requirement;

//         FDS(Graph *graph, int64_t latency_requirement);
//         void run_force_directed_scheduler();
//         void asap_scheduler();
//         void alap_scheduler();
//         void assign_time_frames();
//         void calculate_fds_prob();
//         void calculate_type_dist();
//         void perform_scheduling();
//         double calculate_self_force(int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type, std::string name);
//         double calculate_predecessor_force(Node *node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type);
//         double calculate_successor_force(Node *node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type);
//         void traverse_graph(Node *node, int64_t earlier_time, double &total_successor_force, std::string direction);
//         void update_time_frames(Node *node);

//         void print_asap();
//         void print_alap();
//         void print_time_frames();
//         void print_fds_prob();
//         void print_type_prob();
//         void print_fds_times();
//     };

#endif