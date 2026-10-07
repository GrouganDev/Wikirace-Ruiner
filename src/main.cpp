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
    int rows = 1703505;
    int cols = 2;

    Graph g("../dataset/graphdata_small.bin", rows, cols);

    // Print out the indexes for all articles that article 17 (The Complete History (album)) links to
    for (const int num : g[17]) {
        std::cout << num << " ";
    }

    std::cout << std::endl << std::endl;



    std::cout << "Loading names database from file..." << std::endl;

    // This database maps indices from Graph to the actual names of the articles
    Database names_db("../database/names.db", "SELECT name FROM names WHERE id = ? LIMIT 1");

    std::cout << "Index 17's actual title: " << names_db[17] << std::endl << std::endl;







    // Old Example code for using the largest database file.
    // Not recommended due to slow retrievals. Use Graph instead.
    /*
    std::cout << "Loading large database from file... (this database will likely not be used)" << std::endl;


    // Create your database object and feed it the path to your database file
    // This class will keep track of all internal data for sqlite3 database objects in C++ so lookups are faster
    Database db_large("../database/my_database.db", "SELECT link FROM articles WHERE name = ?");

    // Example code
    std::cout << "Super Mario's hotlinks: " << std::endl;
    // "Super Mario" existed as an article back in 2013 (https://en.wikipedia.org/wiki/Super_Mario)
    for (const std::string& s : db_large["Super Mario"]) {
        std::cout << s << std::endl;
    }

    std::cout << std::endl;

    std::cout << "OpenAI's hotlinks: ";
    // This will print nothing since a wikipedia page for OpenAI didn't exist back in 2013
    for (const std::string& s : db_large["OpenAI"]) {
        std::cout << s << std::endl;
    }

    std::cout << std::endl << std::endl;

    // Print all hotlink titles that were found in the python wikipedia page's third hotlinked article
    std::vector<std::string> python_links = db_large["Python (programming language)"];

    std::cout << "Python's sixth hotlink: " + python_links[5] << std::endl;

    std::cout << std::endl << python_links[5] + "'s hotlinks: " << std::endl;
    for (const std::string& s : db_large[python_links[5]]) {
        std::cout << s << std::endl;
    }

    // Stress test for many queries
    // This method of getting links is not a great idea for large amounts of queries. Use Graph instead for speed.
    for (size_t i = 0; i < 100; ++i) {
        db_large["Python (programming language)"];
    }
    */
}