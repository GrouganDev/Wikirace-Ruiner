//
// Created by matth on 10/4/2026.
//
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

// External library for SQLite functionality (https://www.sqlite.org/)
#include "../lib/sqlite3.h"

#ifndef DSA_PROJECT_2_DATABASE_H
#define DSA_PROJECT_2_DATABASE_H


class Database {
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;
    std::string stmt_txt;
public:
    explicit Database(const std::string& path, const char* query);
    bool changeDatabase(const std::string& path);
    bool changeQuery(const char* query);
    ~Database();

    // Meant for use with "names.db"
    std::string operator[](int index);
    int operator[](const std::string& name);

    // Meant for use with "my_database.db"
    //std::vector<std::string> operator[](const std::string& name);
};



#endif //DSA_PROJECT_2_DATABASE_H
