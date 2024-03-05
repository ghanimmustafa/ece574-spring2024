#ifndef NETLISTPARSER_H
#define NETLISTPARSER_H

#include <string>
#include <vector>
#include <unordered_map>

// Represents a generic component in the netlist (e.g., input, output, wire)
struct Component {
    std::string type;
    std::string name;
    int width;
    bool isSigned;
};

// Represents an operation in the netlist (e.g., ADD, SUB, MUL)
struct Operation {
    std::string opType;
    std::vector<std::string> operands;
    std::string result;
    int width;
    bool isSigned;
};

class NetlistParser {
public:
    NetlistParser(const std::string& filePath);
    void parse();
    const std::vector<Component>& getComponents() const;
    const std::vector<Operation>& getOperations() const;
    std::unordered_map<std::string, int> componentWidths;
    std::unordered_map<std::string, bool> componentSignedness;
   
private:
    std::string filePath;
    std::vector<Component> components;
    std::vector<Operation> operations;
    void parseLine(const std::string& line);
    int determineOperationWidth(const std::string& opType, const std::vector<std::string>& operands); // Ensure this line is in your NetlistParser class
    bool determineOperationSign(const std::string& opType, const std::vector<std::string>& operands);

};


#endif // NETLISTPARSER_H
