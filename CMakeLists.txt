cmake_minimum_required(VERSION 3.16)
project(SQLiteQueryAnalyzer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(app_icon_resource_windows "src/resources/icon.rc")
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(app_icon_macos "src/resources/icon.icns")
set_source_files_properties(${app_icon_macos}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION
    "src/resources")

# Set the executable name based on platform or custom parameter
if(DEFINED EXECUTABLE_NAME)
  set(APP_NAME ${EXECUTABLE_NAME})
else()
  set(APP_NAME SQLiteQueryAnalyzer)
endif()

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

# Snapcraft workaround - skip qt_standard_project_setup as it's not available in older Qt versions
if(COMMAND qt_standard_project_setup)
  qt_standard_project_setup()
endif()

# Define common source list once
set(COMMON_SRCS
  src/main.cpp
  src/database/idatabase.h
  src/database/queryresult.h
  src/database/queryexecutor.cpp
  src/database/queryexecutor.h
  src/database/sqlitedatabase.cpp
  src/database/sqlitedatabase.h
  src/database/inmemorydatabase.cpp
  src/database/inmemorydatabase.h
  src/database/databaseinfo.h
  src/database/dbanalyzer.cpp
  src/database/dbanalyzer.h
  src/database/dbtree.cpp
  src/database/pagedresultmodel.cpp
  src/database/pagedresultmodel.h
  src/database/dbtree.h
  src/gui/highlighter.cpp
  src/gui/highlighter.h
  src/gui/queryexecutionpresenter.cpp
  src/gui/queryexecutionpresenter.h
  src/gui/mainwindow.cpp
  src/gui/mainwindow.h
  src/gui/mainwindow.ui
  src/gui/queryresultpresenter.cpp
  src/gui/queryresultpresenter.h
  src/settings/recentfiles.cpp
  src/settings/recentfiles.h
  src/settings/settings.cpp
  src/settings/settings.h
  src/database/dbexport.cpp
  src/database/dbexport.h
  src/threading/cancellation.cpp
  src/threading/cancellation.h
  src/threading/mainthread.h
  src/database/dbexportschema.cpp
  src/database/dbexportschema.h
  src/database/dbexportdata.cpp
  src/database/dbexportdata.h
  src/database/progress.h
   src/gui/prompts.cpp
   src/gui/prompts.h
   src/gui/sessionmanager.cpp
   src/gui/sessionmanager.h
   src/gui/exportorchestrator.cpp
   src/gui/exportorchestrator.h
  src/cli/export.cpp
  src/cli/export.h
  src/cli/script.cpp
  src/cli/script.h)

if(COMMAND qt_add_executable)
  qt_add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${COMMON_SRCS} $<$<PLATFORM_ID:Windows>:${app_icon_resource_windows}>)
else()
  add_executable(${APP_NAME}
    $<$<PLATFORM_ID:Windows>:WIN32>
    $<$<PLATFORM_ID:Darwin>:MACOSX_BUNDLE>
    ${COMMON_SRCS}
    $<$<PLATFORM_ID:Windows>:${app_icon_resource_windows}>
  )
endif()

target_link_libraries(${APP_NAME} PRIVATE
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

# qt_generate_deploy_app_script(
#         TARGET ${APP_NAME}
#         FILENAME_VARIABLE deploy_script
#         NO_UNSUPPORTED_PLATFORM_ERROR
# )
# install(SCRIPT ${deploy_script})

set(CPACK_PACKAGE_NAME SQLiteQueryAnalyzer)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SQLite Query Analyzer")
set(CPACK_PACKAGE_VENDOR "Christian Resma Helle")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_VERBATIM_VARIABLES ON)
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/sqlitequery")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Christian Resma Helle <christian.helle@outlook.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS libxcb-xinerama0)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/dist/postinst;${CMAKE_CURRENT_SOURCE_DIR}/dist/prerm")
include(CPack)

# Add tests
add_subdirectory(tests)
