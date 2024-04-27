#include "VerilogGenerator.h"
#include <fstream>
#include <sstream>
#include <algorithm> // For std::remove_if
#include <cctype> // For std::isspace
#include <chrono>
#include <ctime>
#include <iostream>
#include <cmath>

std::string decimalToBinary(int decimal, int bitWidth) {
    std::string binary = "";
    while (decimal > 0) {
        int remainder = decimal % 2;
        binary = std::to_string(remainder) + binary;
        decimal /= 2;
    }

    int missingBits = bitWidth - binary.length();
    if (missingBits > 0) {
        binary = std::string(missingBits, '0') + binary;
    }

    return binary;
}

VerilogGenerator::VerilogGenerator(const std::vector<Component>& components, const std::vector<Operation>& operations, Graph* graph)
    : components(components), operations(operations), graph(graph) {}

void VerilogGenerator::generateVerilog(const std::string& outputPath, const std::string& moduleName) {
    std::ofstream outFile(outputPath);
    std::stringstream moduleDecl, declarations;
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::string widthSpecifier;

    // Write system log info
    moduleDecl << "// Generated by University of Arizona - ECE 574 - HLS Tool\n"
            << "// Authors: Mustafa Ghanim and Umut Suluhan\n"
            << "// Date and Time: " << std::ctime(&currentTime)  << "\n"; // ctime includes a newline at the end
    // Start module declaration
    moduleDecl <<"`timescale 1ns / 1ps\n" << "module " << moduleName << "(\n";
    moduleDecl << "\tinput Clk, Rst, Start,\n"; 
    moduleDecl << "\toutput reg Done,\n"; 

    // Process components to generate module IOs and wires
    for (const auto& component : components) {
        std::string signModifier = component.isSigned ? " signed" : "";
        widthSpecifier = (component.width > 1) ? " [" + std::to_string(component.width - 1) + ":0]" : "";
        
        if (component.type == "input") {
            moduleDecl << "\tinput" << signModifier << widthSpecifier << " " << component.name << ",\n";
        } else if (component.type == "output") {
            moduleDecl << "\toutput" << signModifier << widthSpecifier << " " << component.name << ",\n";
        } else if (component.type == "variable") {
            declarations << "\treg" << signModifier << widthSpecifier << " " << component.name << ";\n";
        }
        // else if (component.type == "wire" || component.type == "register") {
        //     declarations << "wire" << signModifier << widthSpecifier << " " << component.name << ";\n";
        // }
    }

    std::vector<int64_t>scheduled_times;
    int64_t state_counter = 0;
    for(const auto& vertex : this->graph->vertices){
        auto it = std::find(scheduled_times.begin(), scheduled_times.end(), vertex->fds_time + 1);
        if (it == scheduled_times.end()) {
            scheduled_times.push_back(vertex->fds_time + 1);
        }
        if (vertex->fds_time > state_counter){
            state_counter = vertex->fds_time;
        }
    }
    std::sort(scheduled_times.begin(), scheduled_times.end());
    state_counter += 3;
    int64_t state_width = std::ceil(std::log2(state_counter));

#if defined(ENABLE_LOGGING)  
    std::cout << "Number of states:" << state_counter << std::endl;
    std::cout << "Width of state register:" << state_width << std::endl;
#endif

    widthSpecifier = (state_width > 1) ? " [" + std::to_string(state_width - 1) + ":0]" : "";
    declarations << "\n\treg" << widthSpecifier << " state" << ";\n";

    std::string binary_state; 
    binary_state = decimalToBinary(0, state_width);
    declarations << "\n\tlocalparam Wait = " << state_width << "'b"<< binary_state << ";\n";
    binary_state = decimalToBinary(state_counter - 1, state_width);
    declarations << "\tlocalparam Final = " << state_width << "'b"<< binary_state << ";\n";

    // Remove the last comma and add closing parenthesis
    std::string moduleDeclStr = moduleDecl.str();
    moduleDeclStr = moduleDeclStr.substr(0, moduleDeclStr.rfind(',')) + "\n);";
    
    outFile << moduleDeclStr << "\n" << declarations.str() << "\n";

    std::stringstream sequential = this->generateSequentialCode(scheduled_times);

    outFile << sequential.str() << "\n";

    outFile << "endmodule\n";

}

std::stringstream VerilogGenerator::generateSequentialCode(std::vector<int64_t>scheduled_times) {
    std::stringstream sequential;

    sequential << "\talways @(posedge clk) begin\n";
    sequential << "\t\tif (rst) begin\n";
    sequential << "\t\t\tstate <= Wait;\n";

    for (const auto& component : components) {
        if (component.type == "output" || component.type == "variable") {
            sequential << "\t\t\t" << component.name << " = 0;\n";
        }
    }
    sequential << "\t\tend\n";
    
    sequential << "\t\telse begin\n";

    sequential << "\t\t\tcase (state)\n";

    sequential << "\t\t\t\tWait: begin\n";
    sequential << "\t\t\t\t\tif (Start == 1) begin\n";
    sequential << "\t\t\t\t\t\tState <= 1;\n";
    sequential << "\t\t\t\t\tend\n";
    sequential << "\t\t\t\t\telse begin\n";
    sequential << "\t\t\t\t\t\tState <= Wait;\n";
    sequential << "\t\t\t\t\tend\n";
    sequential << "\t\t\t\tend\n";

    for(int iter = 0; iter < scheduled_times.size(); iter++){
        int64_t state = scheduled_times.at(iter);
        sequential << "\t\t\t\t" << state << ": begin\n";
        for(const auto& vertex : this->graph->vertices){
            if(vertex->fds_time == state - 1){
                sequential << "\t\t\t\t\t\t" << vertex->operation.line << "\n";
            }
        }
        if(state == scheduled_times.at(scheduled_times.size() - 1)){
            sequential << "\t\t\t\t\t\tstate <= Final;\n";
        }else{
            int64_t next_state = iter + 1;
            sequential << "\t\t\t\t\t\tstate <= " << scheduled_times.at(next_state) <<";\n";
        }
        sequential << "\t\t\t\tend\n";
    }

    sequential << "\t\t\t\tFinal: begin\n";
    sequential << "\t\t\t\t\tDone <= 1;\n";
    sequential << "\t\t\t\t\tState <= Wait;\n";
    sequential << "\t\t\t\tend\n";


    sequential << "\t\t\tendcase\n";

    sequential << "\t\tend\n";


    sequential << "\tend\n";

    return sequential;
    
}