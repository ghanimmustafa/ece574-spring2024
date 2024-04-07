#ifndef NETLISTPARSER_H
#define NETLISTPARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib> // Include at the top of your file
#include <algorithm> // For std::remove_if
#include <cctype> // For std::isspace
#include <unordered_map>
#include <unordered_set>
#include <fstream> // For file I/O operations
#include <iostream>


// Define the map as a static member of the class (or globally if more appropriate for your design)
static std::unordered_map<std::string, std::string> operationSymbols = {
    {"+", "ADD"},
    {"-", "SUB"},
    {"*", "MUL"},
    {"/", "DIV"},
    {"%", "MOD"},
    {"?", "MUX2X1"},
    {">", "COMP"},

    // Add more symbols and their corresponding operation types as needed
};
// Represents a generic component in the netlist (e.g., input, output, wire)
struct Component {
    std::string type;
    std::string name;
    int width;
    bool isSigned;
    bool isReg = false;
    bool isNew = false;
};

// Represents an operation in the netlist (e.g., ADD, SUB, MUL)
struct Operation {
    std::string opType;
    std::string symbol;   
    std::vector<std::string> operands;
    std::string result;
    int width;
    bool isSigned;
    bool isBranch = false;
    std::string condition; // Tag operations with their condition when within a branch
    int state = -1;
    int prev_state = -2;
    std::string name;
};
class OperationGraph;

class NetlistParser {
public:
    NetlistParser(const std::string& filePath,OperationGraph& opGraph);
    void parse();
    const std::vector<Component>& getComponents() const;
    const std::vector<Operation>& getOperations() const;
    std::unordered_map<std::string, int> componentWidths;
    std::unordered_map<std::string, bool> componentSignedness;
    void parseIfOperations(const std::string& ifStatement, const std::string& condition);
    void modifyModuleName(std::string& moduleName);
private:
    std::string filePath;
    OperationGraph& operationGraph; // Reference to an OperationGraph instance
    std::vector<Component> components;
    std::vector<Operation> operations;
    void parseLine(const std::string& line);
    int determineOperationWidth(const std::string& opType, const std::vector<std::string>& operands, const std::string& result); // Ensure this line is in your NetlistParser class
    bool determineOperationSign(const std::string& opType, const std::vector<std::string>& operands, const std::string& result);
    void parseOperation(const std::string& operationLine,const std::string& condition, int state, int prev_state);
    // Existing private members...
    std::string currentCondition; // Track the current conditional context for operations
};


class OperationGraph {
private:
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> edges;
    // Each edge is a pair: target operation and condition
    std::unordered_set<std::string> nodes; // Stores unique node names

public:
    // Add an edge with a condition
    void addEdge(const std::string& from, const std::string& to, const std::string& condition) {
        edges[from].push_back({to, condition});
    }
    // Add a node to the graph
    void addNode(const std::string& nodeName) {
        // Insert the node name into the set of nodes. This ensures each node is unique.
        nodes.insert(nodeName);
    }

    void generateGraphviz(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file for writing.\n";
            return;
        }
        file << "digraph OperationGraph {\n";
        // Declare nodes
        for (const auto& node : nodes) {
            file << "  \"" << node << "\";\n";
        }
        // Declare edges with conditions
        for (const auto& edge : edges) {
            for (const auto& [target, condition] : edge.second) {
                if (!condition.empty()) {
                    file << "  \"" << edge.first << "\" -> \"" << target << "\" [label=\"" << condition << "\"];\n";
                } else {
                    file << "  \"" << edge.first << "\" -> \"" << target << "\";\n";
                }
            }
        }
        file << "}\n";
        file.close();
    }

};
inline bool isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c); });
}
inline bool isExactlyOne(const std::string& str) {
    return str == "1";
}
inline bool isOnlyWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
}
#endif // NETLISTPARSER_H
