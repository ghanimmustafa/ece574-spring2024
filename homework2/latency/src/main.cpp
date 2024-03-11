#include <iostream>
#include <string>
#include <vector>

#include "graph.h" 

int main(int argc, char* argv[]) {

    Graph *graph = new Graph(argv[1]);
    graph->longest_path();

    //std::cout << *graph << std::endl;

    delete graph;

    return 0;
}