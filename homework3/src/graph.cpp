#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "graph.h"

Node::Node(std::string name, std::string type, std::vector<std::string> inputs, std::string output, int64_t datawidth, int64_t latency_requirement) {
    this->name = name;
    this->type = type;
    this->inputs = inputs;
    this->output = output;
    this->datawidth = datawidth;

    this->depth = 1;
    this->asap_time = 0;
    this->alap_time = latency_requirement;
    this->time_frame[0] = 0;
    this->time_frame[1] = 0;
    this->fds_width = 0;

    for (int iter = 0; iter < latency_requirement; iter++) {
        fds_prob.push_back(0.0);
    }
}

// No need to set anything related to scheduling since this constructor is only for source or sink node
Node::Node(std::string name) {
    this->name = name;
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
            ", depth: " << node.depth << ", asap_time: " << node.asap_time << 
            ", alap_time: " << node.alap_time << ", time_frame: [" << node.time_frame[0] <<
            "," << node.time_frame[1] << "]" << ", " << node.fds_width <<  " }";
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
            //if(vertex->type == "REG" && vertex->next.at(0)->name == "Sink"){
            //    vertex->last_reg = 1;
            //}
        }
    }
}

void Graph::generate_components_and_dependencies(std::vector<Operation> operations){

    // Add each datapath component as a node to the graph
    int id = 0;
    for (const auto& operation : operations) {
        //if(operation.opType == "REG"){
            //this->vertices.push_back(new Node(operation.opType + "_" + std::to_string(id), operation.opType, operation.operands, operation.result, operation.width, Reg));
        //}else{
            this->vertices.push_back(new Node(operation.opType + "_" + std::to_string(id), operation.opType, operation.operands, operation.result, operation.width, this->latency_requirement));
        //}
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
        }else if(vertex->next.size() == 0 && vertex->name != "Source" && vertex->name != "Sink"){
            vertex->setNext(this->vertices.at(this->vertices.size() - 1));
            this->vertices.at(this->vertices.size() - 1)->setPrev(vertex);
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

void Graph::asap_scheduler(){
    for (const auto& vertex : this->vertices) {
        vertex->asap_time = vertex->depth;
    }
}

void Graph::alap_scheduler(){
    int alap_time = this->latency_requirement;
    for (int iter = this->vertices.size() - 1; iter >= 0; iter--) {
        int smallest_alap = 1000;
        if(this->vertices.at(iter)->next.size() == 0){
            continue;
        }else{
            for (const auto& vertex : this->vertices.at(iter)->next) {
                if(vertex->alap_time < smallest_alap){
                    smallest_alap = vertex->alap_time;
                }
            }
        this->vertices.at(iter)->alap_time = smallest_alap - 1;
        }
    }
}

void Graph::force_directed_scheduler(){
    this->asap_scheduler();
    this->alap_scheduler();

#if defined(ENABLE_LOGGING)  
    this->print_asap();
    this->print_alap();
#endif
    FDS *fds = new FDS(this, this->latency_requirement);
    fds->run_force_directed_scheduler();

    delete fds;
}

Graph::Graph(int64_t latency_requirement){
    this->latency_requirement = latency_requirement;
}

void Graph::temporary_graph_former(std::string filename){
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        exit(0);
    }

    Node* node1 = nullptr;
    Node* node2 = nullptr;

    std::string line;
    int mode=0;

    while (std::getline(inputFile, line)) {
        std::vector<std::string> words;

        if (line.empty() || line[0] == '#') {
            continue;
        }else{ 
            if(line[0] == '/'){
                mode=1;
                continue;
            }else if(line[0] == '-'){
                mode=2;
                continue;
            }
            if(mode == 1){
                std::istringstream iss(line);
                std::string word;
                while (iss >> word) {
                    words.push_back(word);
                }
                std::vector<std::string>inputs;
                inputs.push_back(words.at(2));
                inputs.push_back(words.at(3));
                if(words.at(1) == "REG"){
                    this->vertices.push_back(new Node(words.at(0), words.at(1), inputs, words.at(4), std::stoll(words.at(5)), this->latency_requirement));
                }else{
                    this->vertices.push_back(new Node(words.at(0), words.at(1), inputs, words.at(4), std::stoll(words.at(5)), this->latency_requirement));
                }
            }else if(mode == 2){
                std::istringstream iss(line);
                std::string word;
                while (iss >> word) {
                    words.push_back(word);
                }

                for (auto& node : this->vertices) {
                    if (node->name == words[0]) {
                        node1 = node;
                    } else if (node->name == words[1]) {
                        node2 = node;
                    }

                    if (node1 && node2) {
                        node1->setNext(node2);
                        if (node1->depth + 1 > node2->depth){
                            node2->depth = node1->depth + 1;
                        } 
                        //std::cout << node1->name << ":" << node2->name << ",," << node2->depth << std::endl;
                        node2->setPrev(node1);
                        break;
                    }
                }
                node1 = nullptr;
                node2 = nullptr;
            }
        }
    }
    inputFile.close();

    delete node1;
    delete node2;

}

void Graph::print_asap(){
    std::cout << "ASAP Scheduling:" << std::endl;
    for (const auto& vertex : this->vertices) {
        std::cout << "Time " << vertex->asap_time << ": " << vertex->name << std::endl;
    }
}

void Graph::print_alap(){
    std::cout << "ALAP Scheduling:" << std::endl;
    for (const auto& vertex : this->vertices) {
        std::cout << "Time " << vertex->alap_time << ": " << vertex->name << std::endl;
    }
}

void Graph::print_time_frames(){
    std::cout << "FDS time frames:" << std::endl;
    for (const auto& vertex : this->vertices) {
        std::cout << vertex->name << ": [" << vertex->time_frame[0] << "," << vertex->time_frame[1] << "]" << ", " << vertex->fds_width <<  std::endl;
    }
}

void Graph::print_fds_prob(){
    std::cout << "FDS operation probabilities:" << std::endl;
    for (const auto& vertex : this->vertices) {
        std::cout << vertex->name << ":" << std::endl; 
        for (int iter = 0; iter < this->latency_requirement; iter++) {
            std::cout << iter+1 << ":" << vertex->fds_prob.at(iter) << std::endl;
        }
    }
}

FDS::FDS(Graph* graph, int64_t latency_requirement){
    this->graph = graph;
    this->latency_requirement = latency_requirement;

    // Might be handled better later...
    // 0: ADD_SUB, 1: MUL, 2: LOG, 3:DIV_MOD
    for(int iter = 0; iter < latency_requirement; iter++){
        this->add_sub_prob.push_back(0.0);
        this->mul_prob.push_back(0.0);
        this->log_prob.push_back(0.0);
        this->div_mod_prob.push_back(0.0);
    }

}

void FDS::run_force_directed_scheduler(){
    this->assign_time_frames();
    this->calculate_fds_prob();
    this->calculate_type_prob();

#if defined(ENABLE_LOGGING)  
    this->graph->print_time_frames();
    this->graph->print_fds_prob();
    this->print_type_prob();
#endif
}

void FDS::assign_time_frames(){
    for (const auto& vertex : this->graph->vertices) {
        vertex->time_frame[0] = vertex->asap_time;
        vertex->time_frame[1] = vertex->alap_time;
        vertex->fds_width = vertex->time_frame[1] - vertex->time_frame[0] + 1;
    }
}

void FDS::calculate_fds_prob(){
    for (const auto& vertex : this->graph->vertices) {
        double prob = 1.0 / vertex->fds_width;
        for(int iter = vertex->time_frame[0]; iter < vertex->time_frame[1]+1; iter++){
            vertex->fds_prob.at(iter-1) = prob;
        }
    }
}

void FDS::calculate_type_prob(){
    for (const auto& vertex : this->graph->vertices) {
        if(vertex->type == "ADD_SUB"){
            for(int iter = 0; iter < this->latency_requirement; iter++){
                this->add_sub_prob.at(iter) += vertex->fds_prob.at(iter);
            }
        }else if(vertex->type == "MUL"){
            for(int iter = 0; iter < this->latency_requirement; iter++){
                this->mul_prob.at(iter) += vertex->fds_prob.at(iter);
            }
        }else if(vertex->type == "LOG"){
            for(int iter = 0; iter < this->latency_requirement; iter++){
                this->log_prob.at(iter) += vertex->fds_prob.at(iter);
            }
        }else if(vertex->type == "DIV_MOD"){
            for(int iter = 0; iter < this->latency_requirement; iter++){
                this->div_mod_prob.at(iter) += vertex->fds_prob.at(iter);
            }
        }else{
            std::cout << "No " << vertex->type << " is not in the type list!!!" << std::endl;
            exit(0);
        }
    }
}

void FDS::print_type_prob(){
    std::cout << "ADD_SUB probabilities:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->add_sub_prob.at(iter) << std::endl;
    }
    std::cout << "MUL probabilities:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->mul_prob.at(iter) << std::endl;
    }
    std::cout << "LOG probabilities:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->log_prob.at(iter) << std::endl;
    }
    std::cout << "DIV_MOD probabilities:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->div_mod_prob.at(iter) << std::endl;
    }
}