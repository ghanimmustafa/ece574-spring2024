#include "NetlistParser.h"
#include "graph.h"
#include <iostream>
#include <filesystem> // C++17 header for file path manipulations
#include <fstream>

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

        delete graph;

        return 0;

    }else if(std::stoi(argv[4]) == 1){

        std::string moduleName = fs::path(cFilePath).stem().string();
        OperationGraph opGraph;

        NetlistParser parser(cFilePath,opGraph);
        parser.modifyModuleName(moduleName);
        parser.parse();

        for (int i = 0; i < parser.getComponents().size(); ++i) {
            std::cout << parser.getComponents()[i].name << ":" << parser.getComponents()[i].type << std::endl;
        }
        std::cout << std::endl;
        
        for (int i = 0; i < parser.getOperations().size(); ++i) {
            std::cout << parser.getOperations()[i].symbol << ":" << parser.getOperations()[i].opType << std::endl;
        }
        std::cout << std::endl;
    // Now, generate the Graphviz file to visualize the operation graph
    // Construct the .dot file name based on moduleName
        std::string dotFileName = moduleName+"_operations_flow.dot";
        std::string pngFileName = moduleName+"_operations_flow.png";
        opGraph.generateGraphviz(moduleName+"_operations_flow.dot");
        std::cout << "Graphviz file generated. Convert 'operation_graph.dot' to an image using Graphviz tools." << std::endl;
        // The command to execute
        std::string command = "dot -Tpng " + dotFileName + " -o " + pngFileName;

        // Execute the command using std::system
        int result = std::system(command.c_str());

        if (result != 0) {
            // The command failed; you can handle the error as needed
            std::cerr << "Failed to execute command: " << command << std::endl;
        }    
        return 0;

    }
}
