#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

#include "graph.h"

Node::Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, int64_t latency_requirement, int64_t latency, Operation operation) {
    this->name = name;
    this->type = type;
    this->inputs = inputs;
    this->output = output;
    this->datawidth = datawidth;
    this->latency = latency;
    this->operation = operation;

    this->asap_time = 1;
    this->alap_time = latency_requirement - latency + 1;
    this->time_frame[0] = 0;
    this->time_frame[1] = 0;
    this->fds_width = 0;
    this->fds_time = 0;

    for (int iter = 0; iter < latency_requirement; iter++) {
        fds_prob.push_back(0.0);
    }
}

void Node::setNext(Node* node) {
   next.push_back(node);
}

void Node::setPrev(Node* node){
    prev.push_back(node);
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    if(node.type.empty()){
        os << "Node { name: " << node.name << " }";
    }else{
        os << "Node { name: " << node.name << ", type: " << node.type;
            for (auto& input : node.inputs) {
                os << ", input: " << input; 
            }
            os << ", output: " << node.output <<", datawidth: " << node.datawidth << 
            ", asap_time: " << node.asap_time << 
            ", alap_time: " << node.alap_time << ", time_frame: [" << node.time_frame[0] <<
            "," << node.time_frame[1] << "]" << ", " << node.fds_width <<
            ", fds_time: " << node.fds_time << " }";
    }
    return os;
}

Graph::Graph(std::vector<Operation> operations, int64_t latency_requirement){
    this->latency_requirement = latency_requirement;

    std::string type;

    for (const Operation& op : operations) {
        if(op.resource == "adder/subtractor"){
            type = "ADD_SUB";
        }else if(op.resource == "logic/logical"){
            type = "LOG";
        }else if(op.resource == "divider/modulo"){
            type = "DIV_MOD";
        }else if(op.resource == "multiplier"){
            type = "MUL";
        }else if(op.name == "source"){
            continue;
        }else if(op.name == "sink"){
            continue;
        }else{  
            std::cout << "No resource like " << op.resource << std::endl;
            exit(0);
        }

        Node *temp = new Node(op.name, type, op.operands, op.result, op.width, latency_requirement, op.cycles, op);
        this->vertices.push_back(temp);
    } 

    this->generate_components_and_dependencies(operations);
}

void Graph::generate_components_and_dependencies(std::vector<Operation> operations){
    for (const Operation& op : operations) {
        for (const auto& vertex : this->vertices) {
            if(op.name == vertex->name){
                for(const auto& successor : op.successors){
                    for (const auto& sub_vertex : this->vertices) {
                        if(successor == sub_vertex->name){
                            vertex->setNext(sub_vertex);
                        }
                    }    
                }
                for(const auto& predecessor : op.predecessors){
                    for (const auto& sub_vertex : this->vertices) {
                        if(predecessor == sub_vertex->name){
                            vertex->setPrev(sub_vertex);
                        }
                    }    
                }
            }
            
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Graph& graph) {
    os << "Latency requirement:" << graph.latency_requirement << std::endl;
    os << "Number of vertices:" << graph.vertices.size() << std::endl;
    for (const auto& vertex : graph.vertices) {
        os << *vertex << std::endl;
        if(vertex->prev.size() != 0){
            for (const auto& sub_vertex : vertex->prev) {
                os << "Prev:" << *sub_vertex << std::endl;
            }
        }
        if(vertex->next.size() != 0){
            for (const auto& sub_vertex : vertex->next) {
                os << "Next:" << *sub_vertex << std::endl;
            }
        }
        os << std::endl;
    }
    os << std::endl;
    return os;
}

