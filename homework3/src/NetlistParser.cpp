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
void trim(std::string &s);
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

/*void NetlistParser::parseIfOperations(const std::string& ifStatement, const std::string& condition) {
    // Here you can parse operations inside the if block and handle the condition
    // For example, you can create Operation objects, calculate width, signedness, etc.
    // For simplicity, I'm just printing the information for demonstration
    std::cout << "Parsing operations inside if block with condition: " << condition << std::endl;
    std::cout << "Statement inside if block: " << ifStatement << std::endl;

    // Parse the condition
    std::istringstream conditionStream(condition);
    std::string leftOperand, opSymbol, rightOperand;
    conditionStream >> leftOperand >> opSymbol >> rightOperand;

    // Handle the condition (you can implement logic based on your requirements)
    std::cout << "Condition parsed: " << leftOperand << " " << opSymbol << " " << rightOperand << std::endl;

    // Parse each line of operations inside the if block
    std::istringstream ifStatementStream(ifStatement);
    std::string line;
    while (getline(ifStatementStream, line)) {
        // Skip empty lines
        if (line.empty())
            continue;

        // Trim leading and trailing whitespaces from the line
        line = trim(line);

        // Check if the line is the closing curly brace '}', indicating the end of the if block
        if (line == "}") {
            break;
        }

        // Parse and handle the operation
        std::cout << "if-based operation: " << line <<std::endl;
        parseOperation(line);
    }
}*/


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
std::unordered_map<int, std::string> lastNodeNameByState;

void NetlistParser::parseBranch(const std::string& branch_type,const std::string& self_condition, const std::string& prev_condition, bool enter_branch, int order, int prev_order) {

    Operation operation;
    operation.condition = prev_condition;
    operation.result = self_condition;
    operation.operands.push_back(self_condition);
    operation.opType = branch_type;
    // Here you might want to call determineOperationWidth or assign a width directly
    operation.order = order;
    operation.prev_order = prev_order;
    std::string nodeName = operation.opType + " v(" + std::to_string(order)+")"; // Construct a unique node name/id
    operation.enter_branch = enter_branch;
    operation.name = nodeName;
    if (operation.opType == "IF")
        operation.line = std::string("if") + " (" +  operation.result + ")" ;     
    operations.push_back(operation);



}



void NetlistParser::parseOperation(const std::string& operationLine,const std::string& condition,bool enter_branch, bool isBranch, int order, int prev_order) {
    std::string beforeEq = operationLine.substr(0, operationLine.find('='));
    std::string afterEq = operationLine.substr(operationLine.find('=') + 1);

    std::string result = beforeEq.substr(0, beforeEq.find(' '));
    std::istringstream afterEqStream(afterEq);

    std::string leftOperand, opSymbol, rightOperand, colon, mux_right;
    afterEqStream >> leftOperand >> opSymbol >> rightOperand >> colon >> mux_right;

    // Check if operands are valid
    if (!isValidOperand(leftOperand, components) || !isValidOperand(result, components) || !isValidOperand(rightOperand, components) || !isValidOperand(mux_right, components)) {
        std::cerr << "Error: Invalid operand in line: " << operationLine << "\n";
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
    operation.enter_branch = enter_branch;
    operation.isBranch = isBranch;
    // Utilize the isExactlyOne function to check operands
    if (opSymbol == "+" && (isExactlyOne(leftOperand) || isExactlyOne(rightOperand))) 
        operation.opType = "INC"; // Increment operation if one operand is exactly "1"
    else if (opSymbol == "-" && (isExactlyOne(leftOperand) || isExactlyOne(rightOperand))) 
        operation.opType = "DEC"; // Decrement operation if one operand is exactly "1"
    else if (opSymbol == "+") 
        operation.opType = "ADD"; // Standard addition for other cases
    else if (opSymbol == "-") 
        operation.opType = "SUB"; // Standard subtraction for other cases
    else if (opSymbol == "*") 
        operation.opType = "MUL";        
    else if (opSymbol == "/") 
        operation.opType = "DIV";
    else if (opSymbol == ">>") 
        operation.opType = "SHR";        
    else if (opSymbol == "<<") 
        operation.opType = "SHL";        
    else if (opSymbol == "%") 
        operation.opType = "MOD"; 
    else if (opSymbol == ">" || opSymbol == "==" || opSymbol == "<") 
        operation.opType = "COMP";        
    else if (opSymbol == "?") {
        operation.opType = "MUX2x1";
        operation.operands.push_back(mux_right);
    }   
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
    operation.width = determineOperationWidth(operation.opType, operation.operands, operation.result);
    operation.isSigned = determineOperationSign(operation.opType, operation.operands, operation.result);
    operation.condition = condition; // Assign the current condition context to the operation
    operation.order = order;
    operation.prev_order = prev_order;
    std::string nodeName = operation.opType + " v(" + std::to_string(order)+")"; // Construct a unique node name/id
    operation.name = nodeName;
    if (operation.opType == "MUX2x1") 
        operation.line = result + " <= " + leftOperand + " " + opSymbol + " " + rightOperand +  " " + colon +  " " + mux_right + ";"; 

    else 
        operation.line = result + " <= " + leftOperand + " " + opSymbol + " " + rightOperand +  ";"; 
    operations.push_back(operation);

    #if defined(ENABLE_LOGGING)
    if (opSymbol == "?") 
        std::cout << "Parsed operation: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand <<  " " << colon <<  " " << mux_right << "\t"; 
    else 
        std::cout << "Parsed operation: " << result << " = " << leftOperand << " " << opSymbol << " " << rightOperand << "\t";
    std::cout << " operation.width:"  << operation.width << "\n";
    std::cout << " operation.condition:"  << operation.condition << "\n";

    #endif

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
// Function to remove whitespace characters from a string
void removeWhitespace(std::string &str) {
    // Use std::remove_if with std::isspace to remove whitespace characters
    size_t resultPos = str.find_first_not_of(" \t");
    if (resultPos != std::string::npos) {
        str.erase(0, resultPos);
    }
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
int order = 0;
int prev_order = -1;
std::stack<std::string> conditionStack;
bool nextIsElse = false;
bool enter_branch = false;
bool isBranch = false;
std::string extractedCondition;
std::string getSecondTop(std::stack<std::string>& stack) {
    if (stack.size() < 2) {
        // Less than two elements means there is no second top element
        return "";
    }

    std::string top = stack.top(); // Temporarily hold the top element
    stack.pop();                   // Remove the top element
    std::string secondTop = stack.top(); // Now the top is the second top
    stack.push(top);               // Restore the original top element

    return secondTop;
}
void NetlistParser::parseLine(const std::string& line) {
    size_t commentPos = line.find("//");

    //std::istringstream stream(line);
    // Extract the part of the line before the comment (if any)
    std::string cleanedLine = (commentPos != std::string::npos) ? line.substr(0, commentPos) : line;

    // Continue parsing only if there's something left after removing the comment
    if (!cleanedLine.empty()) {
        std::istringstream stream(cleanedLine);
        std::string word;
        stream >> word;   

        if (word == "input" || word == "output" || word == "wire" || word == "variable") {
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
                    if(component.type == "variable") component.isReg = true;
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
#if defined(ENABLE_LOGGING)
            std::cout << cleanedLine << std::endl;
#endif
            removeWhitespace(cleanedLine); 
            /*if (cleanedLine.find("if") != std::string::npos) {
                // Correctly extract the condition when entering an 'if' block
                currentCondition = cleanedLine.substr(cleanedLine.find("(") + 1, cleanedLine.find(")") - cleanedLine.find("(") - 1);
                //currentCondition = "g";
                trim(currentCondition);
                // Print the current condition for debugging
                std::cout << "Entering IF block, condition: " << currentCondition << std::endl;
                parseBranch("IF",currentCondition,order++,prev_order++);  

            }
            else if (cleanedLine.find("}") != std::string::npos) {
                // Correctly clear the condition after exiting an 'if' block
                std::cout << "Exiting IF block, clearing condition: " << currentCondition << std::endl;
                currentCondition.clear();
            }
            else{  
              size_t eqPos = cleanedLine.find('=');
              if (eqPos != std::string::npos) {
                parseOperation(cleanedLine,currentCondition,order++,prev_order++);  

              }
            }*/
            /*if (cleanedLine.find("if") != std::string::npos) {
                // Extract the condition from within the parentheses of the 'if'
                size_t start = cleanedLine.find("(") + 1;
                size_t end = cleanedLine.find(")");
                currentCondition = cleanedLine.substr(start, end - start);
                trim(currentCondition);  // Assume trim is a function that removes whitespace

                // Debugging output
                std::cout << "Entering IF block, condition: " << currentCondition << std::endl;

                // Parse the branching operation for entering the 'if'
            
                parseBranch("IF", currentCondition,"", order++, prev_order++);
            }
            else if (cleanedLine.find("}") != std::string::npos) {
                // When exiting the 'if' block
                std::cout << "Exiting IF block, clearing condition: " << currentCondition << std::endl;
                currentCondition.clear();
            }
            else {
                // Handle normal operations inside or outside of 'if' blocks
                size_t eqPos = cleanedLine.find('=');
                if (eqPos != std::string::npos) {
                    parseOperation(cleanedLine, currentCondition, order++, prev_order++);
                }
            }*/
            /*
            if (cleanedLine.find("if") != std::string::npos) {
                // Extract the condition from within the parentheses of the 'if'
                size_t start = cleanedLine.find("(") + 1;
                size_t end = cleanedLine.find(")");
                std::string extractedCondition = cleanedLine.substr(start, end - start);
                trim(extractedCondition);  // Assume trim is a function that removes whitespace

                // Debugging output
                std::cout << "Entering IF block, condition: " << extractedCondition << std::endl;

                // Keep track of the current condition before pushing a new one
                std::string prevCondition;
                if (!conditionStack.empty()) {
                    prevCondition = conditionStack.top();
                }

                // Push the new condition onto the stack
                conditionStack.push(extractedCondition);

                // Parse the branching operation for entering the 'if'
                parseBranch("IF", extractedCondition, prevCondition, order++, prev_order++);
            }
            else if (cleanedLine.find("}") != std::string::npos) {
                // When exiting the 'if' block, ensure there's a condition to pop
                if (!conditionStack.empty()) {
                    std::cout << "Exiting IF block, clearing condition: " << conditionStack.top() << std::endl;
                    conditionStack.pop();
                }

                // Restore previous condition if there is one
                std::string restoredCondition = conditionStack.empty() ? "" : conditionStack.top();
                std::cout << "Condition restored to: " << restoredCondition << std::endl;
            }
            else {
                // Handle normal operations inside or outside of 'if' blocks
                size_t eqPos = cleanedLine.find('=');
                if (eqPos != std::string::npos) {
                    std::string currentCondition = conditionStack.empty() ? "" : conditionStack.top();
                    parseOperation(cleanedLine, currentCondition, order++, prev_order++);
                }
            }                        
            */
            if (cleanedLine.find("if") != std::string::npos) {
                size_t start = cleanedLine.find("(") + 1;
                size_t end = cleanedLine.find(")");
                extractedCondition = cleanedLine.substr(start, end - start);
                trim(extractedCondition);

                // Push the new condition onto the stack
                conditionStack.push(extractedCondition);
                enter_branch = true;
                isBranch = true;
                // Parse the branching operation for entering the 'if'
                parseBranch("IF", extractedCondition, getSecondTop(conditionStack), enter_branch, order++, prev_order++);
                #if defined(ENABLE_LOGGING)  
                std::cout << "Entering IF block, condition: " << extractedCondition << std::endl;
                #endif 
            }
            else if (cleanedLine.find("else") != std::string::npos) {
                enter_branch = false;
                isBranch = true;
                conditionStack.push(extractedCondition);  // Duplicate the last 'if' condition for the 'else' block
                #if defined(ENABLE_LOGGING)  
                std::cout << "Entering ELSE block, using condition of last IF: " << extractedCondition << std::endl;
                #endif 
            }
            else if (cleanedLine.find("}") != std::string::npos) {
                // Reset enter_branch only if exiting a conditional block that was directly active
                if (!conditionStack.empty()) {
                        conditionStack.pop(); // Correctly pop the stack when exiting the conditional block
                }
                #if defined(ENABLE_LOGGING)  
                std::cout << "Exiting block, condition stack size now: " << conditionStack.size() << std::endl;
                #endif 
            }
            else {
                size_t eqPos = cleanedLine.find('=');
                if (eqPos != std::string::npos) {
                    std::string currentCondition = conditionStack.empty() ? "" : conditionStack.top();
                    isBranch = conditionStack.empty() ? false :true;
                    enter_branch = conditionStack.empty() ? false :enter_branch;
                    parseOperation(cleanedLine, currentCondition, enter_branch,isBranch, order++, prev_order++);
                }
            }
            

        }
    }    
}



const std::vector<Component>& NetlistParser::getComponents() const {
    return components;
}

std::vector<Operation>& NetlistParser::getOperations()  {
    return operations;
}

