//
// Created by matth on 10/4/2026.
//

#include "Database.h"

Database::Database(const std::string &path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "FAILED TO OPEN DB";
        db = nullptr;
        return;
    }

    const char* query = "SELECT link FROM articles WHERE name = ?";
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "COULD NOT PREPARE STATEMENT";
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
}

Database::~Database() {
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    if (db) {
        sqlite3_close(db);
    }
}

std::vector<std::string> Database::getHotlinkNames(const std::string &name) {
    std::vector<std::string> result;

    if (!stmt) {
        return result;
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* link = sqlite3_column_text(stmt, 0);
        if (link) {
            result.push_back(std::string(reinterpret_cast<const char*>(link)));
        }
    }

    return result;
}


