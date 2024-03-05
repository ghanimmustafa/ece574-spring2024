#include "NetlistParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib> // Include at the top of your file
#include <algorithm> // For std::remove_if
#include <cctype> // For std::isspace
NetlistParser::NetlistParser(const std::string& filePath) : filePath(filePath) {}

void NetlistParser::parse() {
    std::ifstream file(filePath);
    std::string line;
    while (getline(file, line)) {
        parseLine(line);
    }
}

int NetlistParser::determineOperationWidth(const std::string& opType, const std::vector<std::string>& operands) {
    int maxWidth = 0;
    for (const auto& operand : operands) {
        if (componentWidths.find(operand) != componentWidths.end()) {
            int width = componentWidths[operand];
            maxWidth = std::max(maxWidth, width);
        } else {
            std::cerr << "Error: Operand " << operand << " not found.\n";
            std::exit(EXIT_FAILURE); // Exit if an operand width cannot be determined.
        }
    }
    return maxWidth;
}
bool NetlistParser::determineOperationSign(const std::string& opType, const std::vector<std::string>& operands) {
    for (const auto& operand : operands) {
        // Check if the operand exists in the componentSignedness map
        if (componentSignedness.find(operand) != componentSignedness.end()) {
            // If any operand is signed, the operation is considered signed
            if (componentSignedness[operand]) {
                return true;
            }
        } else {
            std::cerr << "Error: Operand " << operand << " not found.\n";
            std::exit(EXIT_FAILURE); // Exit if an operand's signedness cannot be determined.
        }
    }
    // Return false if none of the operands are signed
    return false;
}

void NetlistParser::parseLine(const std::string& line) {
    if (line.empty() || line[0] == '#' || line.find("//") != std::string::npos) {
        return; // Skip empty lines or comments
    }

    std::istringstream stream(line);
    std::string word;
    stream >> word;

    if (word == "input" || word == "output" || word == "wire") {
        std::string dataType;
        stream >> dataType;
        // Check data type validity
        if (!(dataType.substr(0, 3) == "Int" && dataType.length() > 3 && isdigit(dataType[3])) &&
            !(dataType.substr(0, 4) == "UInt" && dataType.length() > 4 && isdigit(dataType[4]))) {
            std::cerr << "Error: Data type must be 'Int' or 'UInt', found '" << dataType << "' in line: " << line << "\n";
            std::exit(EXIT_FAILURE);
        }      
        int width = std::stoi(dataType.substr(dataType.find_first_of("1234567890")));
      
        bool isSigned = dataType.substr(0, 3) == "Int";

        std::string restOfLine;
        getline(stream, restOfLine, '\n'); // Get the rest of the line after dataType
        std::istringstream restStream(restOfLine);
        std::string componentName;

        while (getline(restStream, componentName, ',')) {
            // Remove spaces from componentName
            componentName.erase(remove_if(componentName.begin(), componentName.end(), isspace), componentName.end());

            if (!componentName.empty()) {
                Component component;
                component.type = word;
                component.name = componentName;
                component.width = width;
                component.isSigned = isSigned;
                components.push_back(component);
                componentWidths[componentName] = width; // Update componentWidths map
                componentSignedness[componentName] = isSigned; // Track signedness of components
                std::cout << "Parsed " << word << ": " << componentName << " with width " << width << (isSigned ? ", signed" : ", unsigned") << "\n";
            }
        }

    } else {
        // Handling operation lines
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string beforeEq = line.substr(0, eqPos);
            std::string afterEq = line.substr(eqPos + 1);
            std::string result = beforeEq.substr(0, beforeEq.find(' '));
            std::istringstream afterEqStream(afterEq);
            std::string leftOperand, opSymbol, rightOperand;

            afterEqStream >> leftOperand >> opSymbol >> rightOperand;

            // Remove semicolon if present in rightOperand
            if (rightOperand.back() == ';')
                rightOperand.pop_back();

            Operation operation;
            operation.result = result;
            operation.operands.push_back(leftOperand);
            operation.operands.push_back(rightOperand);
            
            // Map operation symbols to operation types
            if (opSymbol == "+") operation.opType = "ADD";
            else {
                std::cerr << "Unsupported operation symbol: " << opSymbol << "\n";
                std::exit(EXIT_FAILURE); // Terminate the program
            }

            // Here you might want to call determineOperationWidth or assign a width directly
            operation.width = determineOperationWidth(operation.opType, operation.operands);
            operation.isSigned = determineOperationSign(operation.opType, operation.operands);
            std::cout << " operation.width:"  << operation.width << "\n";
            operations.push_back(operation);
            std::cout << "Parsed operation: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand << "\n";
        }
    }
}



const std::vector<Component>& NetlistParser::getComponents() const {
    return components;
}

const std::vector<Operation>& NetlistParser::getOperations() const {
    return operations;
}
