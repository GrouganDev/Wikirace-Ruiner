//
// Created by matth on 10/4/2026.
//
#include "../lib/sqlite3.h"
#include <vector>
#include <string>
#include <iostream>

#ifndef DSA_PROJECT_2_DATABASE_H
#define DSA_PROJECT_2_DATABASE_H


class Database {
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;
public:
    Database(const std::string& path);
    ~Database();

    std::vector<std::string> getHotlinkNames(const std::string& name);
};


#endif //DSA_PROJECT_2_DATABASE_H