#include "OpPostProcess.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <queue>


OperationGraph OpPostProcess(const std::string& moduleName, const NetlistParser& parser) {
    OperationGraph opGraph;

    // Get the operations from the parser
    const std::vector<Operation>& operations = parser.getOperations();

    // Traverse operations and add nodes to the graph
    for (const auto& op : operations) {
        if (op.opType != "REG") {
            opGraph.addNode(op.name); // Use operation name as node name
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
                        opGraph.addEdge(otherOp.name, op.name, ""); // Add edge if not redundant
                    }
                }
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
        file << "  \"" << node << "\";\n";
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

    return opGraph;
}
