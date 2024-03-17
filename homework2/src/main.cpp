#include "NetlistParser.h"
#include "VerilogGenerator.h"
#include "graph.h"
#include <iostream>
#include <filesystem> // C++17 header for file path manipulations

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <netlist_file> <output_verilog_file>\n";
        return 1;
    }

    std::string netlistFilePath = argv[1];
    std::string outputFilePath = argv[2];
    // Extract the base name of the netlist file without extension to use as the module name
    // Check if the netlist file exists
    if (!std::filesystem::exists(netlistFilePath)) {
        std::cerr << "Error: Netlist file '" << netlistFilePath << "' does not exist.\n";
        return 2; // Return a different error code for this specific error
    }   
    std::string moduleName = fs::path(netlistFilePath).stem().string();

    NetlistParser parser(netlistFilePath);
    parser.parse();
    VerilogGenerator generator(parser.getComponents(), parser.getOperations());
    generator.generateVerilog(outputFilePath, moduleName); // Now correctly passing both arguments
  
    Graph *graph = new Graph(parser.getOperations());
    double critical_path = graph->longest_path();
    
    std::cout << "Critical Path : " << critical_path << " ns" << std::endl << std::endl;  
    std::cout << "Verilog code generated successfully for module " << moduleName << ".\n";
    return 0;
}
