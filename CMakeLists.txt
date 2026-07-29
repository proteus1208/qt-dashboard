cmake_minimum_required(VERSION 3.16)

project(Dashboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        src/PageLayout/HorizentalLayout.cpp
        src/PageLayout/HorizentalLayout.h
        src/PageLayout/VerticalDockBox.cpp
        src/PageLayout/VerticalDockBox.h
        src/PageLayout/HeaderImage.cpp
        src/PageLayout/HeaderImage.h
        src/PageLayout/HeaderDockPanel.cpp
        src/PageLayout/HeaderDockPanel.h
        src/env.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Dashboard
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/assets/assets.qrc
        src/PageLayout/DockWidget.h src/PageLayout/DockWidget.cpp
        src/ListAnimation.h
        src/PageLayout/BorderWidget.h src/PageLayout/BorderWidget.cpp
        src/PageLayout/HoverWidget.h src/PageLayout/HoverWidget.cpp
        src/PageLayout/BorderRateProvider.h
        src/Theme.h
        src/PageLayout/ItemFrame.h src/PageLayout/ItemFrame.cpp
        src/PageLayout/SampleDashboardDock.h src/PageLayout/SampleDashboardDock.cpp
        src/Widgets/Battery.h src/Widgets/Battery.cpp
        src/Widgets/LightButton.h src/Widgets/LightButton.cpp
        src/Widgets/SmoothCurve.h src/Widgets/SmoothCurve.cpp
        src/Widgets/LineChartWidget.h src/Widgets/LineChartWidget.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Dashboard APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Dashboard SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Dashboard
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Dashboard PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(Dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Dashboard)
endif()
set_target_properties(Dashboard PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Dashboard
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Dashboard)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET Dashboard POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Dashboard>/assets"
        COMMENT "Copying assets to output directory")
endif()
