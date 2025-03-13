cmake_minimum_required(VERSION 3.16)
project(SQLiteQueryAnalyzer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(SQLiteQueryAnalyzer WIN32 MACOSX_BUNDLE
    database.cpp database.h
    databaseinfo.h
    dbanalyzer.cpp dbanalyzer.h
    dbquery.cpp dbquery.h
    dbtree.cpp dbtree.h
    highlighter.cpp highlighter.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    recentfiles.cpp recentfiles.h
    ${app_icon_resource_windows}
)
target_link_libraries(SQLiteQueryAnalyzer PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Sql
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(SQLiteQueryAnalyzer PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS SQLiteQueryAnalyzer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET SQLiteQueryAnalyzer
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
