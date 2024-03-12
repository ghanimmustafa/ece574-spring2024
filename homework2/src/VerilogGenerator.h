#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H

#include "NetlistParser.h"
#include <string>

class VerilogGenerator {
public:
    VerilogGenerator(const std::vector<Component>& components, const std::vector<Operation>& operations);
    void generateVerilog(const std::string& outputPath, const std::string& moduleName);

private:
    std::vector<Component> components;
    std::vector<Operation> operations;
    std::string generateComponentCode(const Component& component) const;
    std::string generateOperationCode(const Operation& operation) const;
};


#endif // VERILOGGENERATOR_H
