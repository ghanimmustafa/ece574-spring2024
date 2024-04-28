#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

#include "fds.h"

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

Graph* FDS::run_force_directed_scheduler(){
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
    for(const auto& vertex : this->graph->vertices){
        vertex->fds_time -= 1;
    }

#if defined(ENABLE_LOGGING)  
    this->print_fds_times();
#endif
    for(const auto& vertex : this->graph->vertices){
        if(vertex->next.size() == 0){
            if(vertex->fds_time + vertex->latency <= this->latency_requirement){
                continue;
            }else{
                std::cout << "Couldn't meet the latency requirement of " << this->latency_requirement << " cycles! Increase the latency, exiting ..." << std::endl;
                exit(0);
            }
        }
    }
    return this->graph;
}

void FDS::asap_scheduler(){
    for (const auto& vertex : this->graph->vertices) {
        for(const auto& sub_vertex : vertex->next){
            sub_vertex->asap_time = vertex->asap_time + vertex->latency;
        }
    }
}

void FDS::alap_scheduler(){
    int alap_time = this->graph->latency_requirement;
    for (int iter = this->graph->vertices.size() - 1; iter >= 0; iter--) {
        int smallest_alap = 1000;
        int smallest_alap_latency;
        if(this->graph->vertices.at(iter)->next.size() == 0){
            continue;
        }else{
            for (const auto& vertex : this->graph->vertices.at(iter)->next) {
                if(vertex->alap_time < smallest_alap){
                    smallest_alap = vertex->alap_time;
                    smallest_alap_latency = vertex->latency;
                }
            }
            if(smallest_alap - this->graph->vertices.at(iter)->latency < 0){
                std::cout << "Cannot schedule the circuit for " << this->latency_requirement << " cycle latency , increase the latency! Exiting ..." << std::endl;
                std::cout << "Component is " << this->graph->vertices.at(iter)->name << " and ALAP scheduling is " << smallest_alap << std::endl;
                exit(0); 
            }
            this->graph->vertices.at(iter)->alap_time = smallest_alap - this->graph->vertices.at(iter)->latency;
        }
    }
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
        for(int possible_time = vertex->time_frame[0]; possible_time <= vertex->time_frame[1]; possible_time++){
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

            if(total_force < minimum_total_force){
                if (std::abs(total_force - minimum_total_force) < tolerance) {
                    continue;
                } 
                minimum_total_force = total_force;
                minimum_time = possible_time;
            }
        }
        vertex->fds_time = minimum_time;
        this->update_time_frames(vertex);
        this->calculate_fds_prob();
        this->calculate_type_dist();
    }
}

double FDS::calculate_self_force(int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type, std::string name){
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
        }else if(op_type == "MUL"){
            self_force += this->mul_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
        }else if(op_type == "LOG"){
            self_force += this->log_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
        }else if(op_type == "DIV_MOD"){
            self_force += this->div_mod_prob.at(time - 1) * (calculation_mask[time - time_frame[0]] - fds_prob.at(time-1));
        }else{
            std::cout << "No type named " << op_type << " is present. Exiting at calculate_self_force" << std::endl;
            exit(0);
        }
    }

    return self_force;
}

double FDS::calculate_predecessor_force(Node* node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type){
    double total_predecessor_force = 0.0;
    
    this->traverse_graph(node, possible_time, total_predecessor_force, "prev");
    return total_predecessor_force;
}

double FDS::calculate_successor_force(Node* node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type){
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
        if(direction == "next" && node->time_frame[0] > earlier_time){
            total_successor_force += this->calculate_self_force(node->time_frame[0], node->time_frame, node->fds_prob, node->type, node->name);
        }else if(direction == "prev" && node->time_frame[0] < earlier_time){
            total_successor_force += this->calculate_self_force(node->time_frame[0], node->time_frame, node->fds_prob, node->type, node->name);
        }

        for (const auto& dependentNode : dependencies) {
            traverse_graph(dependentNode, node->time_frame[0], total_successor_force, direction);
        }
    }else{
        for (int iter = node->time_frame[0]; iter <= node->time_frame[1]; iter++) {
            if(direction == "next" && iter > earlier_time){
                total_successor_force += this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name);
            }else if(direction == "prev" && iter < earlier_time){
                total_successor_force += this->calculate_self_force(iter, node->time_frame, node->fds_prob, node->type, node->name);
            }

            for (const auto& dependentNode : dependencies) {
                traverse_graph(dependentNode, iter, total_successor_force, direction);
            }
        }
    }
    
}

void FDS::update_time_frames(Node* node){
    node->time_frame[0] = node->fds_time;
    node->time_frame[1] = node->fds_time;

    if(node->prev.size() != 0){
        for (const auto& vertex : node->prev) {
            if(node->fds_time <= vertex->time_frame[1]){
                vertex->time_frame[1] = node->fds_time - node->latency;
            }
        }
    }

    if(node->next.size() != 0){
        for (const auto& vertex : node->next) {
            if(node->fds_time >= vertex->time_frame[0]){
                vertex->time_frame[0] = node->fds_time + node->latency;
            }
        }
    }
}

