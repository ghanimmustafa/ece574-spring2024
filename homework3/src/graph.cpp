#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

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
    this->fds_time = 0;

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
            "," << node.time_frame[1] << "]" << ", " << node.fds_width <<
            ", fds_time: " << node.fds_time << " }";
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

void Graph::print_fds_times(){
    std::cout << "Scheduled times:" << std::endl;
    for (const auto& vertex : this->vertices) {
        std::cout << vertex->name << ":" << vertex->fds_time << std::endl;
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
    this->calculate_type_dist();

#if defined(ENABLE_LOGGING)  
    this->graph->print_time_frames();
    this->graph->print_fds_prob();
    this->print_type_prob();
#endif

    this->perform_scheduling();


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

void FDS::calculate_type_dist(){
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

void FDS::perform_scheduling(){
    double total_force = 0.0;
    double tolerance = 0.01;
    
    for (const auto& vertex : this->graph->vertices) {
        double minimum_total_force = 10000000.0;
        int64_t minimum_time = 0;
#if defined(ENABLE_LOGGING)  
        std::cout << vertex->name << std::endl;
#endif        
        for(int possible_time = vertex->time_frame[0]; possible_time <= vertex->time_frame[1]; possible_time++){
            // This is the self force of the node when scheduled at possible_time
            double self_force = this->calculate_self_force(possible_time, vertex->time_frame, vertex->fds_prob, vertex->type, vertex->name);
            double predecessor_force = 0.0;
            double successor_force = 0.0;
            if (vertex->prev.size() != 0){
                predecessor_force = this->calculate_predecessor_force(vertex, possible_time, vertex->time_frame, vertex->fds_prob, vertex->type);
            }
            if (vertex->next.size() != 0){
                successor_force = this->calculate_successor_force(vertex, possible_time, vertex->time_frame, vertex->fds_prob, vertex->type);
            }
            total_force = self_force + predecessor_force + successor_force;
#if defined(ENABLE_LOGGING)  
            std::cout << "Time " << possible_time << ": " << self_force << ", PF:" << predecessor_force << ", SF:" << successor_force << std::endl;;
            std::cout << "Total force for " << vertex->name << ": " << total_force << "\n";
#endif 

            if(total_force < minimum_total_force){
                if (std::abs(total_force - minimum_total_force) < tolerance) {
                    continue;
                } 
                minimum_total_force = total_force;
                minimum_time = possible_time;
            }
        }
        // std::cout << *this->graph << "\n\n";
        // std::cout << vertex->name << " scheduled to " << minimum_time << "\n" << std::endl;
        vertex->fds_time = minimum_time;
        this->update_time_frames(vertex);
        // std::cout << *this->graph << "\n\n";
        this->calculate_fds_prob();
        this->calculate_type_dist();
    }
}

double FDS::calculate_self_force(int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type, std::string name){
#if defined(ENABLE_LOGGING)  
        // std::cout << "Calculating self force for " << name << " and time slot " << possible_time << std::endl;
#endif 
    
    double self_force = 0;
    size_t size = time_frame[1] - time_frame[0] + 1;
    int64_t calculation_mask[size];

    for(int iter = 0; iter < size; iter++){
        calculation_mask[iter] = 0;
    }

    calculation_mask[possible_time - time_frame[0]] = 1;

    for(int time = time_frame[0]; time <= time_frame[1]; time++){
        if(op_type == "ADD_SUB"){
            self_force += this->add_sub_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
#if defined(ENABLE_LOGGING)  
            // std::cout << this->add_sub_prob.at(time - 1) << " * " << "(" << calculation_mask[time - time_frame[0]] << " - " << fds_prob.at(time-1) << ")" << " = " << 
                //  this->add_sub_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1)) << std::endl; 
#endif
        }else if(op_type == "MUL"){
            self_force += this->mul_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
#if defined(ENABLE_LOGGING)  
            // std::cout << this->mul_prob.at(time - 1) << " * " << "(" << calculation_mask[time - time_frame[0]] << " - " << fds_prob.at(time-1) << ")" << " = " << 
                        // this->mul_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1)) << std::endl;
#endif
        }else if(op_type == "LOG"){
            self_force += this->log_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
#if defined(ENABLE_LOGGING)  
            // std::cout << this->log_prob.at(time - 1) << " * " << "(" << calculation_mask[time - time_frame[0]] << " - " << fds_prob.at(time-1) << ")" << " = " << 
            //             this->log_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1)) << std::endl;
#endif
        }else if(op_type == "DIV_MOD"){
            self_force += this->div_mod_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
#if defined(ENABLE_LOGGING)  
            // std::cout << this->div_mod_prob.at(time - 1) << " * " << "(" << calculation_mask[time - time_frame[0]] << " - " << fds_prob.at(time-1) << ")" << " = " << 
            //             this->div_mod_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1)) << std::endl;
#endif
        }else{
            std::cout << "No type named " << op_type << " is present. Exiting at calculate_self_force" << std::endl;
            exit(0);
        }
    }

    // std::cout << "DONE and self force is: " << self_force << "\n\n";
    return self_force;
}

double FDS::calculate_predecessor_force(Node* node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type){
#if defined(ENABLE_LOGGING)  
    // std::cout << "Calculating predecessor force for " << node->name << " and time slot " << possible_time << "\n\n";
#endif

    double total_predecessor_force = 0.0;
    
    this->traverse_graph(node, possible_time, total_predecessor_force, "prev");
    return total_predecessor_force;
}

double FDS::calculate_successor_force(Node* node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type){
#if defined(ENABLE_LOGGING)  
    // std::cout << "Calculating successor force for " << node->name << " and time slot " << possible_time << "\n\n";
#endif

    double total_successor_force = 0.0;
    this->traverse_graph(node, possible_time, total_successor_force, "next");
    return total_successor_force;
}

void FDS::traverse_graph(Node *node, int64_t earlier_time, double &total_successor_force, std::string direction){
    std::vector<Node*> dependencies; 

    if(direction == "next"){
        dependencies = node->next;
    }else if (direction == "prev"){
        dependencies = node->prev;
    }else{
        std::cout << "No direction like " << direction << "! Exiting..." << std::endl;
        exit(0);
    }

    if(node->time_frame[0] == node->time_frame[1]){
        // std::cout << "Visiting node: " << node->name << std::endl;
        // std::cout << "Processing time: " << node->time_frame[0]  << ", earlier time:" << earlier_time << std::endl;
        
        if(direction == "next" && node->time_frame[0] > earlier_time){
            // std::cout << "Adding up: " << iter  << ", earlier time:" << earlier_time << std::endl;
            // std::cout << "Successor force:" << this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name) << std::endl;
            total_successor_force += this->calculate_self_force(node->time_frame[0], node->time_frame, node->fds_prob, node->type, node->name);
            //std::cout << "Total successor force:" << total_successor_force << std::endl;
        }else if(direction == "prev" && node->time_frame[0] < earlier_time){
            // std::cout << "Adding up: " << node->time_frame[0]  << ", earlier time:" << earlier_time << std::endl;
            // std::cout << "Predecessor force:" << this->calculate_self_force(node->time_frame[0], node->time_frame, node->fds_prob, node->type, node->name) << std::endl;
            total_successor_force += this->calculate_self_force(node->time_frame[0], node->time_frame, node->fds_prob, node->type, node->name);
            // std::cout << "Total Predecessor force:" << total_successor_force << std::endl;
        }

        for (const auto& dependentNode : dependencies) {
            traverse_graph(dependentNode, node->time_frame[0], total_successor_force, direction);
            // std::cout << "\n";
        }
    }else{
        for (int iter = node->time_frame[0]; iter <= node->time_frame[1]; iter++) {
            // std::cout << "Visiting node: " << node->name << std::endl;
            // std::cout << "Processing time: " << iter  << ", earlier time:" << earlier_time << std::endl;

            if(direction == "next" && iter > earlier_time){
                // std::cout << "Adding up: " << iter  << ", earlier time:" << earlier_time << std::endl;
                // std::cout << "Successor force:" << this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name) << std::endl;
                total_successor_force += this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name);
                //std::cout << "Total successor force:" << total_successor_force << std::endl;
            }else if(direction == "prev" && iter < earlier_time){
                // std::cout << "Adding up: " << iter  << ", earlier time:" << earlier_time << std::endl;
                // std::cout << "Successor force:" << this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name) << std::endl;
                total_successor_force += this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name);
                // std::cout << "Total successor force:" << total_successor_force << std::endl;
            }

            for (const auto& dependentNode : dependencies) {
                traverse_graph(dependentNode, iter, total_successor_force, direction);
                // std::cout << "\n";
            }
        }
    }
    
}

void FDS::update_time_frames(Node* node){
    // std::cout << "Updating time frames of predecessors and successors of " << node->name << "\n\n";

    node->time_frame[0] = node->fds_time;
    node->time_frame[1] = node->fds_time;

    if(node->prev.size() != 0){
        for (const auto& vertex : node->prev) {
            if(node->fds_time <= vertex->time_frame[1]){
                vertex->time_frame[1] = node->fds_time - 1;
            }
        }
    }

    if(node->next.size() != 0){
        for (const auto& vertex : node->next) {
            if(node->fds_time >= vertex->time_frame[0]){
                vertex->time_frame[0] = node->fds_time + 1;
            }
        }
    }
}

void FDS::print_type_prob(){
    std::cout << "ADD_SUB distributions:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->add_sub_prob.at(iter) << std::endl;
    }
    std::cout << "MUL distributions:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->mul_prob.at(iter) << std::endl;
    }
    std::cout << "LOG distributions:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->log_prob.at(iter) << std::endl;
    }
    std::cout << "DIV_MOD distributions:" << std::endl;
    for (int iter = 0; iter < this->latency_requirement; iter++) {
        std::cout << iter+1 << ":" << this->div_mod_prob.at(iter) << std::endl;
    }
}