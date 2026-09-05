cmake_minimum_required(VERSION 3.16)
project(SQLiteQueryTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The test binary compiles Qt sources of its own now -- mainwindow.cpp carries
# Q_OBJECT and mainwindow.ui needs uic -- so switch the generators on here
# rather than inheriting them from the parent project. This directory is
# configurable on its own, and was relying on properties it never set.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Test sources
set(TEST_SRCS
    test_queryexecutor.cpp
    test_sqlidentifier.cpp
    test_dbanalyzer.cpp
    test_dbexportdata.cpp
    test_dbexportschema.cpp
    test_exportorchestrator.cpp
    test_queryresultpresenter.cpp
    test_queryexecutionpresenter.cpp
    test_mainwindow.cpp
    test_pagedresultmodel.cpp
    test_zoomlevel.cpp
    test_zoompresenter.cpp
    test_main.cpp
)

add_executable(${PROJECT_NAME} ${TEST_SRCS})

# Add production source files that tests need
set(PROD_SRCS
    ../src/database/sqlitedatabase.cpp
    ../src/database/inmemorydatabase.cpp
    ../src/database/pagedresultmodel.cpp
    ../src/database/sqldatabaseadapter.cpp
    ../src/database/queryexecutor.cpp
    ../src/database/dbanalyzer.cpp
    ../src/database/dbexportdata.cpp
    ../src/database/dbexportschema.cpp
    ../src/database/dbexport.cpp
    ../src/database/progress.h
    ../src/threading/cancellation.cpp
    ../src/gui/exportorchestrator.cpp
    ../src/gui/queryresultpresenter.cpp
    ../src/gui/queryexecutionpresenter.cpp
    ../src/gui/mainwindow.cpp
    ../src/gui/mainwindow.ui
    ../src/gui/highlighter.cpp
    ../src/gui/sessionmanager.cpp
    ../src/gui/prompts.cpp
    ../src/database/dbtree.cpp
    ../src/settings/settings.cpp
    ../src/settings/recentfiles.cpp
    ../src/gui/zoomlevel.cpp
    ../src/gui/zoompresenter.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${PROD_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Widgets
    GTest::gtest_main
    GTest::gtest
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
