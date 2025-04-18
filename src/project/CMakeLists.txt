cmake_minimum_required(VERSION 3.16)
project(SQLiteQueryAnalyzer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(app_icon_resource_windows "../resources/icon.rc")
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(app_icon_macos "../resources/icon.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "../resources")

# Static build configuration
option(BUILD_STATIC "Build as static library" OFF)
if(BUILD_STATIC OR DEFINED ENV{BUILD_STATIC})
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

# For static builds, we need to include plugin initialization
if(BUILD_STATIC OR DEFINED ENV{BUILD_STATIC})
    # Initialize Qt plugins for static build
    set(CMAKE_AUTOMOC_MACRO_NAMES "Q_OBJECT;Q_GADGET;Q_NAMESPACE;Q_NAMESPACE_EXPORT")
    
    # Add static plugin includes
    set(STATIC_PLUGINS_INCLUDE "
#include <QtPlugin>
#ifdef _WIN32
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
    Q_IMPORT_PLUGIN(QICOPlugin)
#elif defined(__APPLE__)
    Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
    Q_IMPORT_PLUGIN(QICNSPlugin)
#else
    Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
    Q_IMPORT_PLUGIN(QGifPlugin)
    Q_IMPORT_PLUGIN(QICOPlugin)
    Q_IMPORT_PLUGIN(QJpegPlugin)
    Q_IMPORT_PLUGIN(QSvgIconPlugin)
#endif
    Q_IMPORT_PLUGIN(QSQLiteDriverPlugin)
")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/static_plugins.cpp" "${STATIC_PLUGINS_INCLUDE}")
endif()

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
        $<$<OR:$<BOOL:${BUILD_STATIC}>,$<BOOL:$ENV{BUILD_STATIC}>>:${CMAKE_CURRENT_BINARY_DIR}/static_plugins.cpp>
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

# For static builds, link with appropriate plugins
if(BUILD_STATIC OR DEFINED ENV{BUILD_STATIC})
    if(WIN32)
        target_link_libraries(SQLiteQueryAnalyzer PRIVATE
            Qt${QT_VERSION_MAJOR}::QWindowsIntegrationPlugin
            Qt${QT_VERSION_MAJOR}::QICOPlugin
        )
    elseif(APPLE)
        target_link_libraries(SQLiteQueryAnalyzer PRIVATE
            Qt${QT_VERSION_MAJOR}::QCocoaIntegrationPlugin
            Qt${QT_VERSION_MAJOR}::QICNSPlugin
        )
    else()
        target_link_libraries(SQLiteQueryAnalyzer PRIVATE
            Qt${QT_VERSION_MAJOR}::QXcbIntegrationPlugin
            Qt${QT_VERSION_MAJOR}::QGifPlugin
            Qt${QT_VERSION_MAJOR}::QICOPlugin
            Qt${QT_VERSION_MAJOR}::QJpegPlugin
            Qt${QT_VERSION_MAJOR}::QSvgIconPlugin
        )
    endif()
    target_link_libraries(SQLiteQueryAnalyzer PRIVATE
        Qt${QT_VERSION_MAJOR}::QSQLiteDriverPlugin
    )
endif()

install(TARGETS SQLiteQueryAnalyzer
        BUNDLE DESTINATION ..
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/build
)

# Only use deploy script for non-static builds
if(NOT BUILD_STATIC AND NOT DEFINED ENV{BUILD_STATIC})
    qt_generate_deploy_app_script(
            TARGET SQLiteQueryAnalyzer
            FILENAME_VARIABLE deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
