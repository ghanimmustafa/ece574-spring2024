#include "OpPostProcess.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <queue>

void printGraphDetails(const OperationGraph& graph) {
    for (const auto& pair : graph.nodes) {
        pair.second.printDetails();
    }
}


OperationGraph OpPostProcess(const std::string& moduleName, NetlistParser& parser) {
    OperationGraph opGraph;
    Operation sourceOp;
    sourceOp.name = "source";
    sourceOp.opType = "Source"; // Custom type indicating this is a special node
    sourceOp.resource = "None";
    sourceOp.cycles = 0;

    Operation sinkOp;
    sinkOp.name = "sink";
    sinkOp.opType = "Sink"; // Custom type indicating this is a special node
    sinkOp.resource = "None";
    sinkOp.cycles = 0;

    opGraph.addNode(sourceOp.name, sourceOp);
    opGraph.addNode(sinkOp.name, sinkOp);
    // Get the operations from the parser
    std::vector<Operation>& operations = parser.getOperations();
    // Add source and sink operations to the vector
    operations.insert(operations.begin(), sourceOp); // Insert source at the beginning
    operations.push_back(sinkOp);                   // Add sink to the end
    // Traverse operations and add nodes to the graph
    for (auto& op : operations) {
        if(op.opType == "MUL"){
            op.resource = "multiplier";
            op.cycles = 2;
        }
        else if (op.opType == "ADD" || op.opType == "SUB" ||  op.opType == "INC" ||  op.opType == "DEC"){
            op.resource = "adder/subtractor";
            op.cycles = 1;                
        }
        else if (op.opType == "DIV" || op.opType == "MOD" ){
            op.resource = "divider/modulo";
            op.cycles = 3;                
        }
        else if (op.opType == "COMP" || op.opType == "MUX2x1" || op.opType == "SHL" || op.opType == "SHR"){
            op.resource = "logic/logical";
            op.cycles = 1;                
        }                     
        if (op.opType != "REG") {
            opGraph.addNode(op.name,op); // Use operation name as node name
        }


    }


    // Connect nodes based on data dependencies
    /*
    for (const auto& op : operations) {
        // For each operand of the current operation
        for (const auto& operand : op.operands) {
            // Check if the operand is an operation name in the graph
            for (const auto& otherOp : operations) {
                if (otherOp.result == operand) {
                    // Add an edge from operand to current operation
                    opGraph.addEdge(otherOp.name, op.name, ""); // Add edge from operand to current operation
                   
                }
            }
        }
    }
    */
    // Updated loop to construct the graph without adding redundant edges
    for (const auto& op : operations) {
        for (const auto& operand : op.operands) {
            for (const auto& otherOp : operations) {
                if (otherOp.result == operand) {
                    // Before adding an edge, check if 'otherOp' is already reachable from 'op'
                    // This prevents adding redundant edges
                    if (!opGraph.isReachable(otherOp.name, op.name)) {
                        opGraph.addEdge(otherOp.name, op.name, otherOp.result); // Add edge if not redundant
                    }
                }
            }
        }
    }   
    for (auto& [nodeName, node] : opGraph.nodes) {
        if (nodeName != "source" && nodeName != "sink") {
            // Connect to 'source' only if there are no predecessors
            //std::cout << "current node is: " << node.name << std::endl;
            if (node.predecessors.empty()) {
                //opGraph.addEdge("source", nodeName, "");
                opGraph.nodes["source"].successors.push_back(nodeName);
                node.predecessors.push_back("source");
                
            }

            // Connect to 'sink' only if there are no successors
            if (node.successors.empty()) {
                //opGraph.addEdge(nodeName, "sink", "");
                opGraph.nodes["sink"].predecessors.push_back(nodeName);
                node.successors.push_back("sink");
                
            }
        }
    }

    // Generate Graphviz file
    std::string dotFileName = moduleName + "_operations_flow.dot";
    std::string pngFileName = moduleName + "_operations_flow.png";

    std::ofstream file(dotFileName);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return opGraph; // Return the empty graph
    }
    file << "digraph OperationGraph {\n";

    // Declare nodes
    for (const auto& node : opGraph.nodes) {
        file << "  \"" << node.first << "\";\n";
    }

    // Declare edges with conditions
    for (const auto& edge : opGraph.edges) {
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

    // Execute the command to convert the .dot file to an image using Graphviz tools
    std::string command = "dot -Tpng " + dotFileName + " -o " + pngFileName;
    int result = std::system(command.c_str());

    if (result != 0) {
        // The command failed; you can handle the error as needed
        std::cerr << "Failed to execute command: " << command << std::endl;
    }
    //printGraphDetails(opGraph);
    // Assuming you know the number of nodes or have a maximum limit
    // Sort the operations using your predefined function

    return opGraph;
}
