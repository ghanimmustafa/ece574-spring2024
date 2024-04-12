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

// Define OperationGraph directly in OpPostProcess.h
class OperationGraph {
public:
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> edges;
    std::unordered_set<std::string> nodes;

    void addEdge(const std::string& from, const std::string& to, const std::string& condition) {
        edges[from].push_back({to, condition});
    }

    void addNode(const std::string& nodeName) {
        nodes.insert(nodeName);
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
