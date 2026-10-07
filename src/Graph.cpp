#include "Graph.h"

Graph::Graph(const std::string& path, int rows, int cols) {
    this->rows = rows;
    this->cols = cols;




    std::ifstream inFile(path, std::ios::binary);
    if (!inFile) {
        return;
    }

    auto *arr = new int[rows][2];

    inFile.read(reinterpret_cast<char*>(arr), cols * rows * sizeof(int));
    inFile.close();

    int index = arr[0][1];
    std::unique_ptr<std::unordered_set<int>> set = std::make_unique<std::unordered_set<int>>();

    for (int i = 0; i < rows; ++i) {
        if (arr[i][1] != index) {
            map[index] = std::move(set);
            set = std::make_unique<std::unordered_set<int>>();
            index = arr[i][1];
        }

        set->insert(arr[i][0]);
    }

    map[index] = std::move(set);

    delete[] arr;
}

// Return reference to an unordered set which contains all indexes that "index" points to
std::unordered_set<int>& Graph::operator[](const int index) {
    return *map[index];
}
