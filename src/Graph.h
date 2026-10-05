#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#ifndef DSA_PROJECT_2_GRAPH_H
#define DSA_PROJECT_2_GRAPH_H


struct Graph {
    int rows;
    int cols;
    int (*arr)[2] = nullptr;
    explicit Graph(const std::string& path);
    ~Graph();
};




#endif //DSA_PROJECT_2_GRAPH_H
