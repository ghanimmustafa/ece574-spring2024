#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "graph.h"
#include "header.h" 

Node::Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth) {
    this->name = name;
    this->type = type;
    this->inputs = inputs;
    this->output = output;
    this->datawidth = datawidth;
    this->color = Unset;
    this->latency_type = Not_Reg;
    this->distance = 0.0;
    this->init_distance = latency_values[type].values[bidwidths[datawidth]];
    this->last_reg = 0;
}

Node::Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, Latency_type latency_type) {
    this->name = name;
    this->type = type;
    this->inputs = inputs;
    this->output = output;
    this->datawidth = datawidth;
    this->color = Unset;
    this->latency_type = latency_type;
    this->distance = 0.0;
    this->init_distance = latency_values[type].values[bidwidths[datawidth]];
    this->last_reg = 0;
}

Node::Node(std::string name) {
    this->name = name;
    this->color = Unset;
    this->latency_type = Not_Reg;
    this->distance = 0.0;
    this->init_distance = latency_values[type].values[bidwidths[datawidth]];
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
            ", color: " << node.color << ", latency_type: " << node.latency_type << 
            ", distance:" << node.distance << ", last_reg:" << node.last_reg << " }";
    }
    return os;
}

Graph::Graph(std::vector<Operation> operations){

    this->source_node = new Node("Source");
    this->sink_node = new Node("Sink");

    // Generate dependencies and insert source and sink nodes
    this->generate_components_and_dependencies(operations);
    this->vertices.insert(this->vertices.begin(), source_node);
    this->vertices.push_back(sink_node);
    // Resolve dependencies between source-sink node and other nodes 
    this->resolve_dependencies();

    // Label the REGs which are output registers to later exclude them from critical path
    for (const auto& vertex : this->vertices) {
        if(vertex->next.size() != 0){
            if(vertex->type == "REG" && vertex->next.at(0)->name == "Sink"){
                vertex->last_reg = 1;
            }
        }
    }
}

void Graph::generate_components_and_dependencies(std::vector<Operation> operations){

    // Add each datapath component as a node to the graph
    int id = 0;
    for (const auto& operation : operations) {
        if(operation.opType == "REG"){
            this->vertices.push_back(new Node(operation.opType + "_" + std::to_string(id), operation.opType, operation.operands, operation.result, operation.width, Reg));
        }else{
            this->vertices.push_back(new Node(operation.opType + "_" + std::to_string(id), operation.opType, operation.operands, operation.result, operation.width));
        }
        id++;
    }

    // Form the dependencies by looking at inputs and outputs of components 
    for (const auto& producer_vertex : this->vertices) {
        std::string producer_output = producer_vertex->output;
        for (const auto& consumer_vertex : this->vertices) {
            for (const auto& consumer_input : consumer_vertex->inputs) {
                if(consumer_input == producer_output){
                    producer_vertex->setNext(consumer_vertex);
                    consumer_vertex->setPrev(producer_vertex);
                }
            }
        }
    }
}

void Graph::resolve_dependencies(){
    for (const auto& vertex : this->vertices) {
        if(vertex->prev.size() == 0 && vertex->name != "Source" && vertex->name != "Sink"){
            this->vertices.at(0)->setNext(vertex);
            vertex->setPrev(this->vertices.at(0));
            vertex->latency_type = Reg;
        }else if(vertex->next.size() == 0 && vertex->name != "Source" && vertex->name != "Sink"){
            vertex->setNext(this->vertices.at(this->vertices.size() - 1));
            this->vertices.at(this->vertices.size() - 1)->setPrev(vertex);
        }
    }
}

// All operations below are taken from the class notes and detailed information can be found there
// Algorithm in class notes was not taking input-to-register delay into account, we added another attribute called latency_type to support that
// Latency type basically deceives the algorithm by saying that input components are register to include them in the path while they are not
std::vector<Node*> Graph::topological_sort(std::vector<Node*> list, Node* vertex){
    for (const auto& sub_vertex : this->vertices) {
        sub_vertex->color = White;
    }
    list = this->TS_visit(list, vertex);
    return list;
}

std::vector<Node*> Graph::TS_visit(std::vector<Node*> list, Node* vertex){
    vertex->color = Gray;
    for (const auto& adjVertex : vertex->next) {
        if(adjVertex->color == 0 && adjVertex->latency_type == 1){
            list = this->TS_visit(list, adjVertex);
        }else if(adjVertex->color == 0 && adjVertex->latency_type == 0){
            adjVertex->color = Black;
            list.insert(list.begin(), adjVertex);
        }
    }
    vertex->color = Black;
    list.insert(list.begin(), vertex);
    
    return list;
}

double Graph::longest_path(){
    double max = 0;
    for (const auto& vertex : this->vertices) {
        
        if(vertex->latency_type == 0){
            std::vector<Node*> list;
            list = this->topological_sort(list, vertex);
            for (const auto& sub_vertex : list) {
                sub_vertex->distance = sub_vertex->init_distance;
            }
            for (const auto& sub_vertex : list) {
                if((sub_vertex->latency_type == 0 && sub_vertex == vertex) || sub_vertex->latency_type == 1){
                    for (const auto& adjVertex : sub_vertex->next) {
                        if(((sub_vertex->distance + latency_values[adjVertex->type].values[bidwidths[adjVertex->datawidth]]) > adjVertex->distance) && adjVertex->type != "REG"){
                            adjVertex->distance = sub_vertex->distance + latency_values[adjVertex->type].values[bidwidths[adjVertex->datawidth]];
                        }else if(((sub_vertex->distance  + latency_values[adjVertex->type].values[bidwidths[adjVertex->datawidth]]) > adjVertex->distance) && adjVertex->type == "REG"){
                            adjVertex->distance = sub_vertex->distance;
                        }
                    }
                }
            }
            for (const auto& sub_vertex : list) {
                if(sub_vertex->distance > max){
                    max = sub_vertex->distance;
                }
            }
        }
    }
    return max;
}

std::ostream& operator<<(std::ostream& os, const Graph& graph) {
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