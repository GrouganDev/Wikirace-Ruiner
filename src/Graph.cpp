#include "Graph.h"

Graph::Graph(const std::string& path, int rows, int cols) {
    this->rows = rows;
    this->cols = cols;

    std::ifstream inFile(path, std::ios::binary);
    if (!inFile) {
        return;
    }

    const auto arr = new int[rows][2];

    inFile.read(reinterpret_cast<char*>(arr), cols * rows * sizeof(int));
    inFile.close();

    for (int i = 0; i < rows; ++i) {
        int index = arr[i][0];

        map[index].insert(arr[i][1]);
    }

    delete[] arr;
}

// Return reference to an unordered set which contains all indexes that "index" points to
std::unordered_set<int>& Graph::operator[](const int index) {
    return map[index];
}

std::vector<int> Graph::BFS(int start, int target) {
    Timer timer;
    std::queue<int> queue;
    std::unordered_set<int> visited;
    std::unordered_map<int, int> parents;
    visited.insert(start);
    queue.push(start);
    while (!queue.empty()) {
        int v = queue.front();
        queue.pop();

        if (v == target) {
            break;

        }


        for (int neighbor : (*this)[v]) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                queue.push(neighbor);
                parents[neighbor] = v;

            }
        }

    }
    if (visited.find(target) == visited.end()) {
        return {};
    }
    std::vector<int> path;
    int current = target;
    while (current != start) {
        path.push_back(current);
        current = parents[current];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());



    return path;
}

std::vector<int> Graph::dij(int start, int target) {
    Timer timer;
    std::priority_queue<
    std::pair<int, int>,
    std::vector<std::pair<int, int>>,
    std::greater<std::pair<int, int>>> pq;
    std::unordered_map<int, int> distances;
    std::unordered_map<int, int> parents;
    std::unordered_set<int> visited;
    distances[start] = 0;

    pq.push({0, start});
    while (!pq.empty()) {
        int distance = pq.top().first;
        int v = pq.top().second;
        pq.pop();

        visited.insert(v);

        if (distances[v] < distance) {
            continue;
        }
        if (v == target) {
            break;
        }

        for (int neighbor : (*this)[v]) {
            int new_distance = distance+1;
            if (distances.find(neighbor) == distances.end() || new_distance < distances[neighbor]) {
                distances[neighbor] = new_distance;
                parents[neighbor] = v;
                pq.push({new_distance, neighbor});

            }
        }

    }

    std::vector<int> path;
    int current = target;

    if (visited.count(current) == 0) {
        return path;
    }

    while (current != start) {
        path.push_back(current);
        current = parents[current];
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());


    return path;
}

