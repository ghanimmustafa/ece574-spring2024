
#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H

#include "NetlistParser.h"
#include "graph.h"
#include <string>

class VerilogGenerator {
public:
    VerilogGenerator(const std::vector<Component>& components, const std::vector<Operation>& operations, Graph* graph);
    void generateVerilog(const std::string& outputPath, const std::string& moduleName);

private:
    std::vector<Component> components;
    std::vector<Operation> operations;
    Graph* graph;
    std::stringstream generateSequentialCode(int64_t state_counter);
};

#endif // VERILOGGENERATOR_H