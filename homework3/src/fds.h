#ifndef FDS_H
#define FDS_H

#include <string>
#include <vector>

#include "NetlistParser.h"
#include "graph.h"

class FDS{
    public:
        Graph *graph;
        // Might be handled better...
        std::vector<double> add_sub_prob;
        std::vector<double> mul_prob;
        std::vector<double> log_prob;
        std::vector<double> div_mod_prob;

        int64_t latency_requirement;

        FDS(Graph *graph, int64_t latency_requirement);
        void run_force_directed_scheduler();
        void asap_scheduler();
        void alap_scheduler();
        void assign_time_frames();
        void calculate_fds_prob();
        void calculate_type_dist();
        void perform_scheduling();
        double calculate_self_force(int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type, std::string name);
        double calculate_predecessor_force(Node *node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type);
        double calculate_successor_force(Node *node, int64_t possible_time, int64_t time_frame[], std::vector<double> fds_prob, std::string op_type);
        void traverse_graph(Node *node, int64_t earlier_time, double &total_successor_force, std::string direction);
        void update_time_frames(Node *node);

        void print_asap();
        void print_alap();
        void print_time_frames();
        void print_fds_prob();
        void print_type_prob();
        void print_fds_times();
    };

#endif