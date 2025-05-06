cmake_minimum_required(VERSION 3.16)
project(SQLiteQueryAnalyzer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(app_icon_resource_windows "../resources/icon.rc")
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(app_icon_macos "../resources/icon.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "../resources")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(SQLiteQueryAnalyzer WIN32 MACOSX_BUNDLE
        ${app_icon_resource_windows}
        ${app_icon_macos}
        ../database/database.cpp ../database/database.h
        ../database/databaseinfo.h
        ../database/dbanalyzer.cpp ../database/dbanalyzer.h
        ../database/dbquery.cpp ../database/dbquery.h
        ../database/dbtree.cpp ../database/dbtree.h
        ../gui/highlighter.cpp ../gui/highlighter.h
        ../main.cpp
        ../gui/mainwindow.cpp ../gui/mainwindow.h ../gui/mainwindow.ui
        ../settings/recentfiles.cpp ../settings/recentfiles.h
        ../settings/settings.cpp ../settings/settings.h
        ../database/dbexport.cpp ../database/dbexport.h
        ../threading/cancellation.cpp ../threading/cancellation.h
        ../threading/mainthread.h
        ../database/dbexportschema.cpp
        ../database/dbexportschema.h
        ../database/dbexportdata.cpp
        ../database/dbexportdata.h
        ../database/progress.h
        ../gui/prompts.cpp
        ../gui/prompts.h
        ../cli/export.cpp
        ../cli/export.h
        ../cli/script.cpp
        ../cli/script.h
)
target_link_libraries(SQLiteQueryAnalyzer PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Sql
)

if ((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(SQLiteQueryAnalyzer PRIVATE
            Qt::Widgets
    )
endif ()

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

set(CPACK_PACKAGE_NAME SQLiteQueryAnalyzer)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SQLite Query Analyzer")
set(CPACK_PACKAGE_VENDOR "Christian Resma Helle")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_VERBATIM_VARIABLES ON)
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/sqlitequery")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Christian Resma Helle <crhelle@proton.me>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS libc6 libstdc++6 libgcc-s1)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "postinst.sh;prerm.sh")
include(CPack)
