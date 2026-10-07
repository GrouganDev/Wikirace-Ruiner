//
// Created by matth on 10/4/2026.
//

#include "Database.h"

Database::Database(const std::string &path, const char* query) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "FAILED TO OPEN DB";
        db = nullptr;
        return;
    }

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "COULD NOT PREPARE STATEMENT";
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
}

bool Database::changeDatabase(const std::string &path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "FAILED TO OPEN DB";
        db = nullptr;
        return false;
    }

    return true;
}

bool Database::changeQuery(const char *query) {
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "COULD NOT PREPARE STATEMENT";
        sqlite3_finalize(stmt);
        stmt = nullptr;

        return false;
    }

    return true;
}

Database::~Database() {
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    if (db) {
        sqlite3_close(db);
    }
}

std::string Database::operator[](const int index) {
    std::vector<std::string> vec;

    if (!stmt) {
        return "";
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);


    if (sqlite3_bind_text(stmt, 1, std::to_string(index).c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        std::cerr << "INPUTS NOT BOUND TO QUERY: " << sqlite3_errmsg(sqlite3_db_handle(stmt)) << std::endl;
        return "";
    }

    while (sqlite3_step(stmt)) {
        const unsigned char* str = sqlite3_column_text(stmt, 0);
        if (str) {
            std::ostringstream oss;
            oss << str;
            return oss.str();
        }
    }

    return "";
}

std::vector<std::string> Database::operator[](const std::string &name) {
    std::vector<std::string> result;

    if (!stmt) {
        return result;
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        std::cerr << "INPUTS NOT BOUND TO QUERY" << std::endl;
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* link = sqlite3_column_text(stmt, 0);
        if (link) {
            result.emplace_back(reinterpret_cast<const char*>(link));
        }
    }

    return result;
}


