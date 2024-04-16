#include "NetlistParser.h"
#include "graph.h"
#include <iostream>
#include <filesystem> // C++17 header for file path manipulations
#include <fstream>
#include "OpPostProcess.h" // Include the header file for OpPostProcess function

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <cFile> <latency> <verilogFile> <mode> (REMOVE ME LATER!!!)\n";
        return 1;
    }
    std::string cFilePath = argv[1];
    if (!std::filesystem::exists(cFilePath)) {
        std::cerr << "Error: Netlist file '" << cFilePath << "' does not exist.\n";
        return 2; 
    }

    // REMOVE BELOW LATER WHEN EVERYTHING IS MERGED!!!

    if(std::stoi(argv[4]) == 0){ // Umut's mode
        int64_t latency_requirement = std::stoi(argv[2]);
        Graph *graph = new Graph(latency_requirement);
        graph->temporary_graph_former(cFilePath);
        graph->force_directed_scheduler();


        graph->print_fds_times();

        delete graph;

        return 0;

    }else if(std::stoi(argv[4]) == 1){

        std::string moduleName = fs::path(cFilePath).stem().string();

        NetlistParser parser(cFilePath);
        parser.modifyModuleName(moduleName);
        parser.parse();

    // Now, generate the Graphviz file to visualize the operation graph
    // Construct the .dot file name based on moduleName

            // Execute the command using std::system
        for (int i = 0; i < parser.getComponents().size(); ++i) {
            std::cout << parser.getComponents()[i].name << ":" << parser.getComponents()[i].type << std::endl;
        }
        std::cout << std::endl;
        
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
    }


    return 0;

}
