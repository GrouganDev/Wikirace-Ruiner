#include "Database.h"
#include "Graph.h"
#include <string>
#include <iostream>
#include <vector>
#include "../lib/sqlite3.h"
#include <sstream>
#include <fstream>

int main() {
     std::cout << "Loading graph from file..." << std::endl;

    // Loading the large dataset with 4206288 nodes (not recommended for performance)
    /*
    int rows = 101311613;
    int cols = 2;

    Graph g("../datasets/graphdata.bin", rows, cols);
    */



    // Loading the smaller dataset with first 100000 nodes

    // Original dataset file is formatted like a 2D array
    // Each line describes how one article connects to another
    int rows = 1703505;
    int cols = 2;

    Graph g("../DSA-Project-2/dataset/graphdata_small.bin", rows, cols);
    std::cout << "Graph size: " << g.map.size() << std::endl;


    std::cout << std::endl;



    std::cout << "Loading names database from file..." << std::endl << std::endl;

    // This database maps indices from Graph to the actual names of the articles
    Database names_db("../DSA-Project-2/database/names.db", "SELECT name FROM names WHERE id = ? LIMIT 1");

    std::cout << "Index 17's actual title: " << names_db[17] << std::endl << std::endl;


    // Print out the indexes for all articles that article 'index' links to
    int start = g.map.begin()->first;
    int index = *g.map.begin()->second.begin();
    std::vector<int> path = g.BFS(start, index);

    std::cout << "Getting all connections for " << names_db[index] << ":" << std::endl;
    if (g[index].empty()) {
        std::cout << names_db[index] << " has no connections!" << std::endl;
    }
    else {
        for (const int num : g[index]) {
            std::cout << names_db[num] << std::endl;
        }

        std::cout << std::endl;
    }

    if (path.empty()) {
        std::cout << "No path found :(" << std::endl;
    }
    else {
        std::cout << "Shortest path: " << std::endl;
        for (int i = 0; i < path.size(); ++i) {
            std::cout << names_db[path[i]] << (i == path.size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;
    }

    /*
    Graph g("nonexistent.bin", 0, 2);

    g[1].insert(2);
    g[1].insert(3);
    g[2].insert(4);
    g[3].insert(4);
    g[4].insert(5);


    std::vector<int> vec = g.BFS(1, 5);
    for (int i : vec) {
        std::cout << i << std::endl;
    }
    */

}