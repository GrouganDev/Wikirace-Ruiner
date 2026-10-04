#include "Database.h"
#include <string>
#include <iostream>
#include <vector>
#include "../lib/sqlite3.h"

int main() {
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



    for (int i = 0; i < 100; ++i) {
        db.getHotlinkNames("Python (programming language)");
    }
}