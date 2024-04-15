#ifndef OPPOSTPROCESS_H
#define OPPOSTPROCESS_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <fstream>
#include "NetlistParser.h"
#include <stack> // For std::stack
/*struct NodeDetails {
    std::vector<std::string> predecessors;
    std::vector<std::string> successors;
    void print() const {
        std::cout << "Predecessors: ";
        for (const auto& pred : predecessors) {
            std::cout << pred << " ";
        }
        std::cout << "\nSuccessors: ";
        for (const auto& succ : successors) {
            std::cout << succ << " ";
        }
        std::cout << std::endl;
    }    
};*/



// Define OperationGraph directly in OpPostProcess.h
class OperationGraph {
public:
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> edges;
    //std::unordered_set<std::string> nodes;
    //std::unordered_map<std::string, NodeDetails> nodes;  // Changed from std::unordered_set<std::string>
    std::unordered_map<std::string, Operation> nodes;  // Stores Operation structs including predecessor and successor info

    /*void addEdge(const std::string& from, const std::string& to, const std::string& condition) {
        edges[from].push_back({to, condition});
    }*/
    void addEdge(const std::string& from, const std::string& to, const std::string& condition) {
        if (!isReachable(from, to)) {
            edges[from].push_back({to, condition});
            nodes[from].successors.push_back(to);
            nodes[to].predecessors.push_back(from);

            // Print the connection that is being added
            /*std::cout << "Added edge from " << from << " to " << to << " with condition: " << condition << std::endl;
            std::cout << " - " << from << " next nodes: ";
            for (const auto& succ : nodes[from].successors) std::cout << succ << " ";
            std::cout << std::endl;
            std::cout << " - " << to << " previous nodes: ";
            for (const auto& pred : nodes[to].predecessors) std::cout << pred << " ";
            std::cout << std::endl;*/
        }
    }    
    std::vector<Operation> sortOperations(const std::unordered_map<std::string, Operation>& nodes) {
        std::vector<Operation> sortedOperations;

        // Ensure source and sink are present
        if (nodes.find("source") == nodes.end() || nodes.find("sink") == nodes.end()) {
            throw std::runtime_error("Source or sink node missing");
        }

        // Reserve space for efficiency, excluding source and sink
        sortedOperations.reserve(nodes.size() - 2);

        // Add all operations except source and sink
        for (const auto& pair : nodes) {
            if (pair.first != "source" && pair.first != "sink") {
                sortedOperations.push_back(pair.second);
            }
        }

        // Sort operations by 'order'
        std::sort(sortedOperations.begin(), sortedOperations.end(), [](const Operation& a, const Operation& b) {
            return a.order < b.order;
        });

        // Insert 'source' at the beginning and 'sink' at the end
        sortedOperations.insert(sortedOperations.begin(), nodes.at("source"));
        sortedOperations.push_back(nodes.at("sink"));

        return sortedOperations;
    }

    /*void addNode(const std::string& nodeName) {
        nodes.insert(nodeName);
    }*/
    /*void addNode(const std::string& nodeName) {
        if (nodes.find(nodeName) == nodes.end()) {
            nodes[nodeName] = NodeDetails();  // Initialize with empty NodeDetails
        }
    }*/
    // This method should accept an Operation object, not just the name
    void addNode(const std::string& nodeName, const Operation& op) {
        nodes[nodeName] = op; // Directly store the passed Operation object
    }

    void printConnectionsInOrder();

    void generateGraphviz(const std::string& filename);
    bool isReachable(const std::string& from, const std::string& to) {
        if (from == to) return true; // Early return if the nodes are the same

        std::unordered_set<std::string> visited;
        std::stack<std::string> stack;
        stack.push(from);

        while (!stack.empty()) {
            std::string current = stack.top();
            stack.pop();

            if (visited.find(current) != visited.end()) {
                continue;
            }
            visited.insert(current);

            auto it = edges.find(current);
            if (it == edges.end()) {
                continue;
            }

            for (const auto& edge : it->second) {
                if (edge.first == to) {
                    return true;
                }
                stack.push(edge.first);
            }
        }

        return false;
    }
private:
    void findAllReachable(const std::string& start, std::unordered_set<std::string>& visited) {
        for (const auto& edge : edges[start]) {
            if (visited.insert(edge.first).second) { // Successfully inserted (not yet visited)
                findAllReachable(edge.first, visited);
            }
        }
    }  

};

// Declaration of OpPostProcess function
OperationGraph OpPostProcess(const std::string& moduleName,  NetlistParser& parser);
#endif // OPPOSTPROCESS_H
