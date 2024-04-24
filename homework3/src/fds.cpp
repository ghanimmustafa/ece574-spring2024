#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

#include "fds.h"

void FDS::asap_scheduler(){
    for (const auto& vertex : this->graph->vertices) {
        std::cout << *vertex << std::endl;
        for(const auto& sub_vertex : vertex->next){
            std::cout << *sub_vertex << std::endl;
        }
        std::cout << std::endl;
    }
    exit(0);

}

void FDS::alap_scheduler(){
    int alap_time = this->graph->latency_requirement;
    for (int iter = this->graph->vertices.size() - 1; iter >= 0; iter--) {
        int smallest_alap = 1000;
        if(this->graph->vertices.at(iter)->next.size() == 0){
            continue;
        }else{
            for (const auto& vertex : this->graph->vertices.at(iter)->next) {
                if(vertex->alap_time < smallest_alap){
                    smallest_alap = vertex->alap_time;
                }
            }
        this->graph->vertices.at(iter)->alap_time = smallest_alap - 1;
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
    this->asap_scheduler();
    this->alap_scheduler();
    this->assign_time_frames();
    this->calculate_fds_prob();
    this->calculate_type_dist();

#if defined(ENABLE_LOGGING)  
    this->print_asap();
    this->print_alap();
    this->print_time_frames();
    this->print_fds_prob();
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

