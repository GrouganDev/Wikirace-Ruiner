#include "db.h"

std::string exec(const char* cmd) {
    // Use unique_ptr to automatically handle pclose()
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    std::string result;
    char buffer[128];

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    return result;
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}


std::vector<std::string> getHotlinkNames(const std::string& name) {
    std::string output = exec(("cd ../ ; venvLinux/bin/python3 src/python/db.py \"" + name + "\"").c_str());
    return split(output, '\n');
}

