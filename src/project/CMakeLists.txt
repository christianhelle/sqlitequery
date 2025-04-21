cmake_minimum_required(VERSION 3.16)
project(SQLiteQueryAnalyzer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Static build configuration
option(BUILD_STATIC "Build with static Qt libraries" OFF)
if(BUILD_STATIC)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.lib")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_LINK_SEARCH_START_STATIC ON)
    set(CMAKE_LINK_SEARCH_END_STATIC ON)
endif()

set(app_icon_resource_windows "../resources/icon.rc")
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(app_icon_macos "../resources/icon.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "../resources")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
if(BUILD_STATIC)
    set(QT_STATIC TRUE)
endif()
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

# For static builds on Windows, we need additional platform libraries
if(BUILD_STATIC AND WIN32)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WinExtras)
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

# For static builds on Windows, link additional platform libraries
if(BUILD_STATIC AND WIN32)
    target_link_libraries(SQLiteQueryAnalyzer PRIVATE
        Qt::WinExtras
        dwmapi.lib
        imm32.lib
        version.lib
        winmm.lib
        ws2_32.lib
    )
endif()

# For static builds on Linux, we need these additional libraries
if(BUILD_STATIC AND UNIX AND NOT APPLE)
    target_link_libraries(SQLiteQueryAnalyzer PRIVATE
        -lX11
        -lxcb
        -lXau
        -lXdmcp
        -ldl
        -lfontconfig
        -lfreetype
        -lpng
        -lz
        -lexpat
    )
endif()

install(TARGETS SQLiteQueryAnalyzer
        BUNDLE DESTINATION ..
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/build
)

qt_generate_deploy_app_script(
        TARGET SQLiteQueryAnalyzer
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
