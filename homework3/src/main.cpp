#include <iostream>
#include <filesystem> // C++17 header for file path manipulations
#include <fstream>

#include "OpPostProcess.h" 
#include "NetlistParser.h"
#include "graph.h"
#include "fds.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <cFile> <latency> <verilogFile>\n";
        return 1;
    }
    std::string cFilePath = argv[1];
    if (!std::filesystem::exists(cFilePath)) {
        std::cerr << "Error: Netlist file '" << cFilePath << "' does not exist.\n";
        return 2; 
    }

    int64_t latency_requirement = std::stoi(argv[2]);
    std::string moduleName = fs::path(cFilePath).stem().string();

    NetlistParser parser(cFilePath);
    //parser.modifyModuleName(moduleName);
    parser.parse();

    // Now, generate the Graphviz file to visualize the operation graph
    // Construct the .dot file name based on moduleName

    // Execute the command using std::system
    
    std::cout << "GetComponents" << std::endl;
    for (int i = 0; i < parser.getComponents().size(); ++i) {
        std::cout << parser.getComponents()[i].name << ":" << parser.getComponents()[i].type << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "GetOperations" << std::endl;
    for (int i = 0; i < parser.getOperations().size(); ++i) {
        std::cout << parser.getOperations()[i].symbol << ":" << parser.getOperations()[i].opType << std::endl;
    }
    std::cout << std::endl;
    // Now, call the OpPostProcess function passing the module name

    OperationGraph opGraph = OpPostProcess(moduleName, parser);   
    std::vector<Operation> sortedOperations = opGraph.sortOperations(opGraph.nodes);

    // Print details of each operation after sorting
    for (const Operation& op : sortedOperations) {
        op.printDetails();
    }          

    Graph *graph = new Graph(sortedOperations, latency_requirement);
    FDS *fds = new FDS(graph, latency_requirement);
    fds->run_force_directed_scheduler();

    delete graph;
    delete fds;

    return 0;
}