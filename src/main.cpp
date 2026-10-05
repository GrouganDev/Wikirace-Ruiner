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

    Graph g("../datasets/graphdata.bin");

    int rows = g.rows;
    int cols = g.cols;

    // Print the first 10 connections of the graph
    for (int i = 0; i < 10; ++i) {
        std::cout << g.arr[i][0] << " " << g.arr[i][1] << std::endl;
    }

    std::cout << std::endl;


    // Create your database object and feed it the path to your database file
    // This class will keep track of all internal data for sqlite3 database objects in C++ so lookups are faster
    Database db("../database/my_database.db");

    // Example code
    std::cout << "Super Mario's hotlinks: " << std::endl;
    // "Super Mario" existed as an article back in 2013 (https://en.wikipedia.org/wiki/Super_Mario)
    for (const std::string& s : db.getHotlinkNames("Super Mario")) {
        std::cout << s << std::endl;
    }

    std::cout << std::endl;

    std::cout << "OpenAI's hotlinks: ";
    // This will print nothing since a wikipedia page for OpenAI didn't exist back in 2013
    for (const std::string& s : db.getHotlinkNames("OpenAI")) {
        std::cout << s << std::endl;
    }

    std::cout << std::endl << std::endl;

    // Print all hotlink titles that were found in the python wikipedia page's third hotlinked article
    std::vector<std::string> python_links = db.getHotlinkNames("Python (programming language)");

    std::cout << "Python's third hotlink: " + python_links[2] << std::endl;

    std::cout << std::endl << python_links[2] + "'s hotlinks: ";
    for (const std::string& s : db.getHotlinkNames(python_links[2])) {
        std::cout << s << std::endl;
    }


    // Stress test for many queries
    for (int i = 0; i < 100; ++i) {
        db.getHotlinkNames("Python (programming language)");
    }
}