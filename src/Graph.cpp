#include "Graph.h"

Graph::Graph(const std::string& path) {
    rows = 101311613;
    cols = 2;


    std::ifstream inFile("../dataset/graphdata.bin", std::ios::binary);
    if (!inFile) {
        return;
    }

    arr = new int[rows][2];

    inFile.read(reinterpret_cast<char*>(arr), rows * cols * sizeof(int));
    inFile.close();
}

Graph::~Graph() {
    delete[] arr;
}
