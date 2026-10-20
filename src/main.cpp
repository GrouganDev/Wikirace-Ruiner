#include "Database.h"
#include "Graph.h"
#include <string>
#include <iostream>
#include <vector>
#include "../lib/sqlite3.h"
#include <sstream>
#include <fstream>

int main() {
    /* std::cout << "Loading graph from file..." << std::endl;

    // Loading the large dataset with 4206288 nodes (not recommended for performance)
    /*
    int rows = 101311613;
    int cols = 2;

    Graph g("../datasets/graphdata.bin", rows, cols);
    */


    // Loading the smaller dataset with first 100000 nodes

    // Original dataset file is formatted like a 2D array
    // Each line describes how one article connects to another
    /* int rows = 1703505;
    int cols = 2;

    Graph g("../dataset/graphdata_small.bin", rows, cols);


    std::cout << std::endl;



    std::cout << "Loading names database from file..." << std::endl << std::endl;

    // This database maps indices from Graph to the actual names of the articles
    Database names_db("../database/names.db", "SELECT name FROM names WHERE id = ? LIMIT 1");

    std::cout << "Index 17's actual title: " << names_db[17] << std::endl << std::endl;


    // Print out the indexes for all articles that article 'index' links to

    int index = 64;

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
    */
    Graph g("nonexistent.bin", 0, 2);

    g[1].insert(2);
    g[1].insert(3);
    g[2].insert(4);
    g[3].insert(4);
    g[4].insert(5);

    std::cout << std::boolalpha;
    std::cout << g.BFS(1, 5) << std::endl; // true
    std::cout << g.BFS(5, 1) << std::endl; // false

}