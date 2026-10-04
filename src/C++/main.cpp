#include "db.h"
#include <string>
#include <iostream>
#include <vector>

int main() {
    for (const std::string& s : getHotlinkNames("Super Mario")) {
        std::cout << s << std::endl;
    }
}