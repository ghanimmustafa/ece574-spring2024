#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "graph.h"
#include "header.h" 

Node::Node(std::string name, std::string type, std::string input1, std::string input2, std::string output, int64_t datawidth) {
    this->name = name;
    this->type = type;
    this->input1 = input1;
    this->input2 = input2;
    this->output = output;
    this->datawidth = datawidth;
    this->color = Unset;
    this->latency_type = Not_Reg;
    this->distance = 0.0;
    this->init_distance = latency_values[type].values[bidwidths[datawidth]];
}

Node::Node(std::string name, std::string type, std::string input1, std::string input2, std::string output, int64_t datawidth, Latency_type latency_type) {
    this->name = name;
    this->type = type;
    this->input1 = input1;
    this->input2 = input2;
    this->output = output;
    this->datawidth = datawidth;
    this->color = Unset;
    this->latency_type = latency_type;
    this->distance = 0.0;
    this->init_distance = latency_values[type].values[bidwidths[datawidth]];
}

Node::Node(std::string name) {
    this->name = name;
    this->color = Unset;
    this->latency_type = Not_Reg;
    this->distance = 0.0;
    this->init_distance = latency_values[type].values[bidwidths[datawidth]];
}

void Node::setNext(Node* node) {
   next.push_back(node);
}

void Node::setPrev(Node* node){
    prev.push_back(node);
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    if(node.type.empty()){
        os << "Node { name: " << node.name << " }";
    }else{
        os << "Node { name: " << node.name << ", type: " << node.type <<
            ", input1: " << node.input1 << ", input2: " << node.input2 << 
            ", output: " << node.output <<", datawidth: " << node.datawidth <<
            ", color: " << node.color << ", latency_type: " << node.latency_type << 
            ", distance:" <<node.distance << " }";
    }
    return os;
}

void Graph::form_graph(std::string file_name){
    std::ifstream inputFile(file_name);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        exit(0);
    }

    Node* node1 = nullptr;
    Node* node2 = nullptr;

    std::string line;
    int mode=0;

    while (std::getline(inputFile, line)) {
        std::vector<std::string> words;

        if (line.empty() || line[0] == '#') {
            continue;
        }else{ 
            if(line[0] == '/'){
                mode=1;
                continue;
            }else if(line[0] == '-'){
                mode=2;
                continue;
            }
            if(mode == 1){
                std::istringstream iss(line);
                std::string word;
                while (iss >> word) {
                    words.push_back(word);
                }
                if(words.at(1) == "REG"){
                    this->vertices.push_back(new Node(words.at(0), words.at(1), words.at(2), words.at(3), words.at(4), std::stoll(words.at(5)), Reg));
                }else{
                    this->vertices.push_back(new Node(words.at(0), words.at(1), words.at(2), words.at(3), words.at(4), std::stoll(words.at(5))));
                }
            }else if(mode == 2){
                std::istringstream iss(line);
                std::string word;
                while (iss >> word) {
                    words.push_back(word);
                }

                for (auto& node : this->vertices) {
                    if (node->name == words[0]) {
                        node1 = node;
                    } else if (node->name == words[1]) {
                        node2 = node;
                    }

                    if (node1 && node2) {
                        node1->setNext(node2);
                        node2->setPrev(node1);
                        break;
                    }
                }
                node1 = nullptr;
                node2 = nullptr;
            }
        }
    }
    inputFile.close();

    delete node1;
    delete node2;

}

void Graph::resolve_dependencies(){
    for (const auto& element : this->vertices) {
        if(element->prev.size() == 0 && element->name != "Source" && element->name != "Sink"){
            this->vertices.at(0)->setNext(element);
            element->setPrev(this->vertices.at(0));
            element->latency_type = Reg;
        }else if(element->next.size() == 0 && element->name != "Source" && element->name != "Sink"){
            element->setNext(this->vertices.at(this->vertices.size() - 1));
            this->vertices.at(this->vertices.size() - 1)->setPrev(element);
        }
    }
}

std::vector<Node*> Graph::topological_sort(std::vector<Node*> list, Node* vertex){
    for (const auto& element : this->vertices) {
        element->color = White;
    }
    list = this->TS_visit(list, vertex);
    return list;
}

std::vector<Node*> Graph::TS_visit(std::vector<Node*> list, Node* vertex){
    vertex->color = Gray;
    for (const auto& adjVertex : vertex->next) {
        if(adjVertex->color == 0 && adjVertex->latency_type == 1){
            list = this->TS_visit(list, adjVertex);
        }else if(adjVertex->color == 0 && adjVertex->latency_type == 0){
            adjVertex->color = Black;
            list.insert(list.begin(), adjVertex);
        }
    }
    vertex->color = Black;
    list.insert(list.begin(), vertex);
    
    return list;
}

void Graph::longest_path(){
    float max = 0;
    for (const auto& vertex : this->vertices) {
        
        if(vertex->latency_type == 0){
            std::vector<Node*> list;
            list = this->topological_sort(list, vertex);
            for (const auto& sub_vertex : list) {
                //std::cout << *sub_vertex << std::endl;
                sub_vertex->distance = sub_vertex->init_distance;
            }
            for (const auto& sub_vertex : list) {
                if((sub_vertex->latency_type == 0 && sub_vertex == vertex) || sub_vertex->latency_type == 1){
                    //std::cout << "SUB:" <<*sub_vertex << std::endl; 
                    for (const auto& adjVertex : sub_vertex->next) {
                        //std::cout << "ADJ:" << *adjVertex << std::endl; 
                        if((sub_vertex->distance /*+ latency_values[sub_vertex->type].values[sub_vertex->datawidth]*/ + latency_values[adjVertex->type].values[bidwidths[adjVertex->datawidth]]) > adjVertex->distance){
                            adjVertex->distance = sub_vertex->distance /*+ latency_values[sub_vertex->type].values[sub_vertex->datawidth]*/ + latency_values[adjVertex->type].values[bidwidths[adjVertex->datawidth]];
                            //std::cout << "IN:" << *adjVertex << std::endl; 
                        }
                    }
                    //std::cout << std::endl;
                }
            }
            for (const auto& sub_vertex : list) {
                if(sub_vertex->latency_type == 0 && sub_vertex->distance > max){
                    max = sub_vertex->distance;
                }
            }
            //std::cout << std::endl << std::endl;
        }
    }
    std::cout << max << std::endl;
}

Graph::Graph(std::string file_name){
    this->source_node = new Node("Source");
    this->sink_node = new Node("Sink");

    this->vertices.push_back(source_node);
    this->form_graph(file_name);
    this->vertices.push_back(sink_node);

    this->resolve_dependencies();
}

std::ostream& operator<<(std::ostream& os, const Graph& graph) {
    os << "Number of vertices:" << graph.vertices.size() << std::endl;
    for (const auto& element : graph.vertices) {
        os << *element << std::endl;
        if(element->prev.size() != 0){
            for (const auto& element1 : element->prev) {
                os << "Prev:" << *element1 << std::endl;
            }
        }
        if(element->next.size() != 0){
            for (const auto& element1 : element->next) {
                os << "Next:" << *element1 << std::endl;
            }
        }
        os << std::endl;
    }
    os << std::endl;
    return os;
}