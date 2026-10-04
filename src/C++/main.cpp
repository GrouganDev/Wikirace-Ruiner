#include "db.h"
#include <string>
#include <iostream>
#include <vector>

int main() {
    for (const std::string& s : getHotlinkNames("Python (programming language)")) {
        std::cout << s << std::endl;
    }
}