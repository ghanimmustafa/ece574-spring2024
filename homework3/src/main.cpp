#include "NetlistParser.h"
#include <iostream>
#include <filesystem> // C++17 header for file path manipulations

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <cFile> <latency> <verilogFile>\n";
        return 1;
    }

    std::string cFilePath = argv[1];
    //std::string outputFilePath = argv[2];

    if (!std::filesystem::exists(cFilePath)) {
        std::cerr << "Error: Netlist file '" << cFilePath << "' does not exist.\n";
        return 2; 
    }   
    std::string moduleName = fs::path(cFilePath).stem().string();

    NetlistParser parser(cFilePath);
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

    return 0;
}
