#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

#include "graph.h"
#include "fds.h"

void FDS::print_asap(){
    std::cout << "ASAP Scheduling:" << std::endl;
    for (const auto& vertex : this->graph->vertices) {
        std::cout << "Time " << vertex->asap_time << ": " << vertex->name << std::endl;
    }
    std::cout << "\n";
}

void FDS::print_alap(){
    std::cout << "ALAP Scheduling:" << std::endl;
    for (const auto& vertex : this->graph->vertices) {
        std::cout << "Time " << vertex->alap_time << ": " << vertex->name << std::endl;
    }
    std::cout << "\n";
}

void FDS::print_time_frames(){
    std::cout << "FDS time frames:" << std::endl;
    for (const auto& vertex : this->graph->vertices) {
        std::cout << vertex->name << ": [" << vertex->time_frame[0] << "," << vertex->time_frame[1] << "]" << ", " << vertex->fds_width <<  std::endl;
    }
    std::cout << "\n";
}

void FDS::print_fds_prob(){
    std::cout << "FDS operation probabilities:" << std::endl;
    for (const auto& vertex : this->graph->vertices) {
        std::cout << vertex->name << ":" << std::endl; 
        for (int iter = 0; iter < this->latency_requirement; iter++) {
            std::cout << iter+1 << ":" << vertex->fds_prob.at(iter) << std::endl;
        }
    }
    std::cout << "\n";
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
    std::cout << "\n";
}

void FDS::print_fds_times(){
    std::cout << "Scheduled times:" << std::endl;
    for (const auto& vertex : this->graph->vertices) {
        std::cout << vertex->name << ":" << vertex->fds_time << std::endl;
    }
    std::cout << "\n";
}
