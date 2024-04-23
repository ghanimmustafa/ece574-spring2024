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
#include <queue>
#include <locale>

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
    std::string line;   
    std::vector<std::string> operands;
    std::string result;
    int width;
    bool isSigned;
    bool isBranch = false;
    std::string condition; // Tag operations with their condition when within a branch
    int order = -1;
    int prev_order = -2;
    std::string name;
    std::string fds_type;
    int cycles=0;
    std::string resource;
    std::vector<std::string> predecessors;
    std::vector<std::string> successors;
    // Function to print operation details
   void printDetails() const {
        std::cout << "Operation Name: " << name << "\t"
                  << "Expression: " << line << "\t"
                  << "Inputs: ";

        if (!operands.empty()) {
            for (size_t i = 0; i < operands.size() - 1; ++i) {
                std::cout << operands[i] << ", ";
            }
            std::cout << operands.back();
        }

        std::cout << "\t"
                  << "Output: " << result << "\t"
                  << "Type: " << opType << "\t"
                  << "Resource: " << resource << "\t"
                  << "Cycles: " << cycles << "\t"
                  << "Predecessors: ";

        for (const auto& pred : predecessors) {
            std::cout << pred << " ";
        }

        std::cout << "\tSuccessors: ";
        for (const auto& succ : successors) {
            std::cout << succ << " ";
        }
        std::cout << std::endl;
    }       
};

class NetlistParser {
public:
    NetlistParser(const std::string& filePath);
    void parse();
    const std::vector<Component>& getComponents() const;
    std::vector<Operation>& getOperations() ;
    std::unordered_map<std::string, int> componentWidths;
    std::unordered_map<std::string, bool> componentSignedness;
    void parseIfOperations(const std::string& ifStatement, const std::string& condition);
    void modifyModuleName(std::string& moduleName);
private:
    std::string filePath;
    std::vector<Component> components;
    std::vector<Operation> operations;
    void parseLine(const std::string& line);
    int determineOperationWidth(const std::string& opType, const std::vector<std::string>& operands, const std::string& result); // Ensure this line is in your NetlistParser class
    bool determineOperationSign(const std::string& opType, const std::vector<std::string>& operands, const std::string& result);
    void parseOperation(const std::string& operationLine,const std::string& condition, int state, int prev_state);
    void parseBranch(const std::string& branch_type,const std::string& self_condition, const std::string& prev_condition,int order, int prev_order); 
    // Existing private members...
    std::string currentCondition; // Track the current conditional context for operations
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


// Trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
#endif // NETLISTPARSER_H
