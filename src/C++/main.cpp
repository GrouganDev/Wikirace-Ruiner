#include "db.h"
#include <string>
#include <iostream>
#include <vector>

int main() {

    // Example code

    // "Super Mario" existed as an article back in 2013 (https://en.wikipedia.org/wiki/Super_Mario)
    for (const std::string& s : getHotlinkNames("Super Mario")) {
        std::cout << s << std::endl;
    }

    // This will print nothing since a wikipedia page for OpenAI didn't exist back in 2013
    for (const std::string& s : getHotlinkNames("OpenAI")) {
        std::cout << s << std::endl;
    }
}