cmake_minimum_required(VERSION 3.28)
project(DSA_Project_2)

set(CMAKE_CXX_STANDARD 14)

add_executable(DSA_Project_2
        src/main.cpp
        lib/sqlite3.h
        lib/sqlite3.c
        src/Database.cpp
        src/Database.h
        src/Graph.cpp
        src/Graph.h)
