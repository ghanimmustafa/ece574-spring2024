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
            // After processing all operations
            
    for (auto& component : components) {
        // Check if the component is an output and hasn't been registered
        if (component.type == "output" && !component.isReg) {
            // Check if the wire component already exists
            std::string wireOperand = component.name + "wire";
            bool wireExists = std::any_of(components.begin(), components.end(), [&](const Component& comp) { return comp.name == wireOperand; });

            // If the wire component doesn't exist, add it to the components
            if (!wireExists) {
                Component wireComponent;
                wireComponent.type = "wire";
                wireComponent.name = wireOperand;
                wireComponent.width = component.width; // Set the width according to the component
                wireComponent.isSigned = component.isSigned; // Set the signedness according to the component
                wireComponent.isNew = true;
                components.push_back(wireComponent);

                #if defined(ENABLE_LOGGING)  
                std::cout << "Declared wire component: " << wireOperand << " with width " << component.width << (component.isSigned ? ", signed" : ", unsigned") << "\n";
                #endif 
            }

            // Add a registration operation for the output component
            Operation regOperation;
            regOperation.result = component.name;
            regOperation.operands.push_back(wireOperand);
            regOperation.opType = "REG";
            regOperation.width = component.width; // Set the width according to the component
            regOperation.isSigned = component.isSigned; // Set the signedness according to the component
            operations.push_back(regOperation);

            // Set the isReg flag for the output component
            component.isReg = true;
            // Replace other operations using the output as an operand with the new wire         
        }
 
    }
 
    // Replace operands in operations with the new wire if necessary
    for (auto& operation : operations) {
        for (auto& operand : operation.operands) {
            // Check if operand matches any output component that has been registered
            auto it = std::find_if(components.begin(), components.end(), [&](const Component& comp) {
                return comp.name == operand && comp.type == "output" && comp.isReg;
            });

            if (it != components.end()) {
                // If found, replace the operand with its corresponding wire
                std::string wireOperand = it->name + "wire";
                operand = wireOperand;
                std::cerr << "Multi-driven pin detected and resolved for " << it->name << ". Replaced with " << wireOperand << std::endl;
            }
        }
    }
    #if defined(ENABLE_LOGGING)  
    for (const auto& component : components) {
        std::cout << "Component: " << component.name << ", isReg: " << (component.isReg ? "true" : "false") << std::endl;
    }
    #endif       
}
bool isNumeric(const std::string& str);
bool isExactlyOne(const std::string& str);  
bool isOnlyWhitespace(const std::string& str);

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

void NetlistParser::modifyModuleName(std::string& moduleName) {
    // Check for leading digits and remove them
    moduleName.erase(0, moduleName.find_first_not_of("0123456789"));

    // Remove special characters and the character before them
    for (size_t i = 0; i < moduleName.size(); ++i) {
        if (!std::isalnum(moduleName[i])) {
            if (i > 0) {
                moduleName.erase(i - 1, 1); // Remove the character before the special character
                // Update 'i' as we removed a character
                --i;
            }
            moduleName.erase(i, 1); // Remove the special character
            // Decrement 'i' to recheck the character that moved into the current position
            --i;
        }
    }
}


int NetlistParser::determineOperationWidth(const std::string& opType, const std::vector<std::string>& operands, const std::string& result) {
    int maxWidth = 0;

    if (opType == "COMP") {
        // Determine the maximum width among operands
        for (const auto& operand : operands) {
            if (isNumeric(operand) || isOnlyWhitespace(operand)) continue; // Skip constants or empty operands

            if (componentWidths.find(operand) != componentWidths.end()) {
                int width = componentWidths[operand];
                maxWidth = std::max(maxWidth, width);
            } else {
                std::cerr << "Error: Operand " << operand << " not found.\n";
                std::exit(EXIT_FAILURE); // Exit if an operand width cannot be determined.
            }
        }
    } else {
        // Determine the width based on the result
        if (componentWidths.find(result) != componentWidths.end()) {
            maxWidth = componentWidths[result];
        } else {
            std::cerr << "Error: Result " << result << " not found.\n";
            std::exit(EXIT_FAILURE); // Exit if the result width cannot be determined.
        }
    }

    return maxWidth;
}



/*
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

*/
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

bool isValidOperand(const std::string& operand, const std::vector<Component>& components) {
    // Check if the operand is a constant or whitespace
    if (isNumeric(operand) || isOnlyWhitespace(operand))
        return true;

    // Check if the operand is inside the components
    for (const auto& component : components) {
        if (operand == component.name)
            return true;
    }

    // Operand is neither constant, whitespace, nor inside components
    return false;
}

void NetlistParser::parseLine(const std::string& line) {
    /*if (line.empty() || line[0] == '#' || line.find("//") != std::string::npos) {
        return; // Skip empty lines or comments
    }*/
    size_t commentPos = line.find("//");

    //std::istringstream stream(line);
    // Extract the part of the line before the comment (if any)
    std::string cleanedLine = (commentPos != std::string::npos) ? line.substr(0, commentPos) : line;

    // Continue parsing only if there's something left after removing the comment
    if (!cleanedLine.empty()) {
        std::istringstream stream(cleanedLine);
        std::string word;
        stream >> word;   
    //std::string word;
    //stream >> word;

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
                    if(width == 1) component.isSigned = false;
                    else component.isSigned = isSigned;
                    components.push_back(component);
                    componentWidths[componentName] = width; // Update componentWidths map
                    componentSignedness[componentName] = isSigned; // Track signedness of components
                    #if defined(ENABLE_LOGGING)
                    std::cout << "Parsed " << word << ": " << componentName << " with width " << width << (component.isSigned ? ", signed" : ", unsigned") << "\n"; 
                    #endif
                }
            }

        } else {
            // Handling operation lines
            size_t eqPos = cleanedLine.find('=');
            if (eqPos != std::string::npos) {
                std::string beforeEq = cleanedLine.substr(0, eqPos);
                std::string afterEq = cleanedLine.substr(eqPos + 1);
                std::string result = beforeEq.substr(0, beforeEq.find(' '));
                std::istringstream afterEqStream(afterEq);
                std::string leftOperand, opSymbol, rightOperand,colon, mux_right;

                afterEqStream >> leftOperand >> opSymbol >> rightOperand >> colon >>  mux_right;
                // Check if operands are valid
                if (!isValidOperand(leftOperand, components) || !isValidOperand(result, components) || !isValidOperand(rightOperand, components) || !isValidOperand(mux_right, components)) {
                    std::cerr << "Error: Invalid operand in line: " << line << "\n";
                    std::exit(EXIT_FAILURE);
                }          
                // Remove semicolon if present in rightOperand
                if (!rightOperand.empty() && rightOperand.back() == ';')
                    rightOperand.pop_back();
		if (!mux_right.empty() && mux_right.back() == ';')
                    mux_right.pop_back();      
                Operation operation;
                operation.result = result;
                operation.operands.push_back(leftOperand);
                operation.operands.push_back(rightOperand);
                operation.symbol = opSymbol;
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
                else if (opSymbol == "") {
                    // Find the corresponding component
                    auto resultComponent = std::find_if(components.begin(), components.end(), [&](const Component& comp) { return comp.name == result; });

                    // If the result component is found and is registered, assign operation type
                    if (resultComponent != components.end() && !resultComponent->isReg){ 
                        operation.opType = "REG";
                        resultComponent->isReg = true;
                    }    
                }    
                
                else {
                    std::cerr << "Unsupported operation symbol: " << opSymbol << "\n";
                    std::exit(EXIT_FAILURE); // Terminate the program for unsupported symbols
                }

                // Here you might want to call determineOperationWidth or assign a width directly
                operation.width = determineOperationWidth(operation.opType, operation.operands,operation.result);
                operation.isSigned = determineOperationSign(operation.opType, operation.operands,operation.result);
                operations.push_back(operation);
                #if defined(ENABLE_LOGGING)
                std::cout << " operation.width:"  << operation.width << "\n";
              
                std::cout << "Parsed operation: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand << "\n";
                if (opSymbol == "?") 
                std::cout << "Parsed MUX: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand <<  " " << colon <<  " " << mux_right << "\n";
                #endif
            }

            

        }
    }    
}



const std::vector<Component>& NetlistParser::getComponents() const {
    return components;
}

const std::vector<Operation>& NetlistParser::getOperations() const {
    return operations;
}

