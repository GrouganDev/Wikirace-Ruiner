find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(wikiruiner_tests
    IntegrationTests.cpp
    ../../src/Graph.cpp
    ../../src/Timer.cpp
    ../../lib/sqlite3.c
)
set_source_files_properties(../Main.qml PROPERTIES QT_RESOURCE_ALIAS Main.qml)
qt_add_qml_module(wikiruiner_tests
    URI untitled
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/untitled"
    QML_FILES ../Main.qml
    SOURCES ../SearchController.h ../SearchController.cpp
)
target_include_directories(wikiruiner_tests PRIVATE ..)
target_compile_definitions(wikiruiner_tests PRIVATE
    WIKIRUINER_DATA_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/../.."
)
target_link_libraries(wikiruiner_tests PRIVATE
    Qt6::Quick Qt6::QuickControls2 Qt6::Concurrent Qt6::Test
)
add_test(NAME wikiruiner_integration COMMAND wikiruiner_tests)
set(test_environment "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software")
if(WIN32)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tmp")
    list(APPEND test_environment
        "QT_QPA_FONTDIR=$ENV{WINDIR}/Fonts"
        "TEMP=${CMAKE_CURRENT_BINARY_DIR}/tmp"
        "TMP=${CMAKE_CURRENT_BINARY_DIR}/tmp"
    )
endif()
set_tests_properties(wikiruiner_integration PROPERTIES
    ENVIRONMENT "${test_environment}"
)
