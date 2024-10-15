cmake_minimum_required(VERSION 3.16)

project(after_lines VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Quick3D)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appafter_lines
    main.cpp
)

set_target_properties(appafter_lines PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(appafter_lines PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Quick3D
)

qt_add_qml_module(appafter_lines
    URI linesExample
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        axisLine.h axisLine.cpp
        axis.h    axis.cpp
        timerworker.h timerworker.cpp
        timerthread.h timerthread.cpp
    RESOURCE_PREFIX "/qt/qml"
    IMPORTS
        QtQuick3D

)

install(TARGETS appafter_lines
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET appafter_lines
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})
