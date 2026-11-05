cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Concurrent)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(wikirace_ruiner
    main.cpp
    ../src/Graph.cpp
    ../src/Timer.cpp
    ../lib/sqlite3.c
)

qt_add_qml_module(wikirace_ruiner
    URI untitled
    QML_FILES
        Main.qml
    SOURCES
        SearchController.h
        SearchController.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(wikirace_ruiner PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.wikirace_ruiner
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(wikirace_ruiner
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Concurrent
)

target_include_directories(wikirace_ruiner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(wikirace_ruiner PRIVATE
    WIKIRUINER_DATA_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/.."
)

install(TARGETS wikirace_ruiner
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

option(WIKIRUINER_BUILD_TESTS "Build GUI and search integration tests" OFF)
if(WIKIRUINER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
