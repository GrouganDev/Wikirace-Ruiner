#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>

#ifndef DSA_PROJECT_2_GRAPH_H
#define DSA_PROJECT_2_GRAPH_H


struct Graph {
    int rows;
    int cols;
    std::unordered_map<int, std::unordered_set<int>> map;
    explicit Graph(const std::string& path, int rows, int cols);

    std::unordered_set<int>& operator[](int index);

    std::vector<int> BFS(int start, int target);
};




#endif //DSA_PROJECT_2_GRAPH_H
