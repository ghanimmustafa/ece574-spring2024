#include "NetlistParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

NetlistParser::NetlistParser(const std::string& filePath) : filePath(filePath) {}

void NetlistParser::parse() {
    std::ifstream file(filePath);
    std::string line;
    while (getline(file, line)) {
        parseLine(line);
    }
}
bool isNumeric(const std::string& str);
bool isExactlyOne(const std::string& str);  
bool isOnlyWhitespace(const std::string& str);

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>



void NetlistParser::postprocessVerilogCode(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::set<std::string> outputs;
    std::set<std::string> registered;

    // Read through the file to collect outputs and registered outputs
    while (getline(file, line)) {
        // Check if the line declares an output
        if (line.find("output") != std::string::npos) {
            auto startPos = line.find_last_of(' ') + 1;
            auto endPos = line.find(';');
            std::string outputName = line.substr(startPos, endPos - startPos);
            outputs.insert(outputName);
        }
        // Check if the line contains a register instantiation for an output
        if (line.find("REG") != std::string::npos && line.find(".q(") != std::string::npos) {
            auto startPos = line.find(".q(") + 3;
            auto endPos = line.find(")", startPos);
            std::string regName = line.substr(startPos, endPos - startPos);
            registered.insert(regName);
        }
    }
    file.close();

    // Append register instantiations for unregistered outputs
    std::ofstream outFile(filePath, std::ios_base::app);
    for (const auto& output : outputs) {
        if (registered.find(output) == registered.end()) {
            // This output is not registered; append a REG module for it
            outFile << "REG #(.DATAWIDTH(32)) " << output << "_reg (.d(" << output << "Wire), .Clk(Clk), .Rst(Rst), .q(" << output << "));\n";
        }
    }

    outFile.close();
    std::cout << "Post-processed Verilog file: " << filePath << std::endl;
}



int NetlistParser::determineOperationWidth(const std::string& opType, const std::vector<std::string>& operands,const std::string& result) {
    int maxWidth = 0;
    if (componentWidths.find(result) != componentWidths.end()) {
        maxWidth = componentWidths[result];
    }    
    for (const auto& operand : operands) {
        if (isNumeric(operand) || isOnlyWhitespace(operand)) continue; // Skip constants or empty operands

        std::cout << "Operand: " << operand << "\n"; // Debugging print   
        if (componentWidths.find(operand) != componentWidths.end()) {
            int width = componentWidths[operand];
            maxWidth = std::max(maxWidth, width);
        } else {
            std::cerr << "Error: Operand/Result " << operand << " not found.\n";
            std::exit(EXIT_FAILURE); // Exit if an operand width cannot be determined.
        }
    }
    return maxWidth;
}
bool NetlistParser::determineOperationSign(const std::string& opType, const std::vector<std::string>& operands,const std::string& result) {
     if (componentSignedness.find(result) != componentSignedness.end() && componentSignedness[result]) {
        return true; // Operation is signed if the result is signed
    }   
    for (const auto& operand : operands) {
        // Check if the operand exists in the componentSignedness map
        if (isNumeric(operand) || isOnlyWhitespace(operand)) continue; // Skip constants or empty operands
        if (componentSignedness.find(operand) != componentSignedness.end()) {
            // If any operand is signed, the operation is considered signed
            if (componentSignedness[operand]) {
                return true;
            }
        } else {
            std::cerr << "Error: Operand/Result " << operand << " not found.\n";
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

    if (word == "input" || word == "output" || word == "wire" || word == "register") {
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
            std::string leftOperand, opSymbol, rightOperand,colon, mux_right;

            afterEqStream >> leftOperand >> opSymbol >> rightOperand >> colon >>  mux_right;
           
            // Remove semicolon if present in rightOperand
            if (rightOperand.back() == ';')
                rightOperand.pop_back();
            if (mux_right.back() == ';')
                mux_right.pop_back();      
            Operation operation;
            operation.result = result;
            operation.operands.push_back(leftOperand);
            operation.operands.push_back(rightOperand);
            operation.symbol = opSymbol; 
            std:: cout << opSymbol << std::endl;
            // Utilize the isExactlyOne function to check operands
            if (opSymbol == "+" && (isExactlyOne(leftOperand) || isExactlyOne(rightOperand))) operation.opType = "INC"; // Increment operation if one operand is exactly "1"
            else if (opSymbol == "-" && (isExactlyOne(leftOperand) || isExactlyOne(rightOperand))) operation.opType = "DEC"; // Decrement operation if one operand is exactly "1
            else if (opSymbol == "+") operation.opType = "ADD"; // Standard addition for other cases
            else if (opSymbol == "-") operation.opType = "SUB"; // Standard subtraction for other cases
            else if (opSymbol == "*") operation.opType = "MUL";        
            else if (opSymbol == "/") operation.opType = "DIV";
            else if (opSymbol == ">>") operation.opType = "SHR";        
            else if (opSymbol == "<<") operation.opType = "SHL";        
            else if (opSymbol == "%") operation.opType = "MOD";        
            else if (opSymbol == ">" || opSymbol == "==" || opSymbol == "<") operation.opType = "COMP";        
            else if (opSymbol == "?") {operation.opType = "MUX2x1";operation.operands.push_back(mux_right);}   
            else if (opSymbol == "") operation.opType = "REG";        

            else {
                std::cerr << "Unsupported operation symbol: " << opSymbol << "\n";
                std::exit(EXIT_FAILURE); // Terminate the program for unsupported symbols
            }

            // Here you might want to call determineOperationWidth or assign a width directly
            operation.width = determineOperationWidth(operation.opType, operation.operands,operation.result);
            operation.isSigned = determineOperationSign(operation.opType, operation.operands,operation.result);
            std::cout << " operation.width:"  << operation.width << "\n";
            operations.push_back(operation);
            std::cout << "Parsed operation: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand << "\n";
            if (opSymbol == "?") 
            std::cout << "Parsed MUX: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand <<  " " << colon <<  " " << mux_right << "\n";
          
        }
    }
}

const std::vector<Component>& NetlistParser::getComponents() const {
    return components;
}

const std::vector<Operation>& NetlistParser::getOperations() const {
    return operations;
}

