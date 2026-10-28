cmake_minimum_required(VERSION 3.16)

project(FoilBrowser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(/Ox /GF)
    add_compile_options(/MT)
endif()

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Qml
    WebEngineQuick
    Multimedia
)
qt_standard_project_setup(REQUIRES 6.8)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

list(APPEND QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/qml")

qt_add_executable(FoilBrowser
    WIN32
    main.cpp
)

set(QML_SRC_FILES
    Main.qml
    WebTab.qml
    PageLoaderPopup.qml
    DownloadPage.qml
    WebDialog.qml
    PermissionPage.qml
    ColorAdjustmentPage.qml
)

qt_add_qml_module(FoilBrowser
    URI "App.Core"
    VERSION 1.0
    QML_FILES
    ${QML_SRC_FILES}
    SOURCES
    utils.hpp
    qquickstack.hpp
    simpleRect.hpp

    DEPENDENCIES
    QComponentsproj
    WebEngineQuick
    QtQuick
    QtCore
    QtQml

    NO_GENERATE_AOT_VALIDATION
)
add_subdirectory("C:/Libraries/QML/QComponents" QComponentsproj)

target_link_libraries( FoilBrowser PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::WebEngineQuick
    Qt6::Multimedia
    QComponentsprojplugin
)

set_target_properties(FoilBrowser PROPERTIES QT_QML_GENERATE_META_TYPES ON)

if(WIN32)
    set(INSTALL_BINDIR ".")
else()
    set(INSTALL_BINDIR "bin")
endif()

install(TARGETS FoilBrowser
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${INSTALL_BINDIR}
)

