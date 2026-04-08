cmake_minimum_required(VERSION 3.16)

project(taquin_ui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Concurrent)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(apptaquin_ui
    main.cpp
    taquin_core.cpp
    taquin_solver.cpp
    taquin_game.cpp
)

qt_add_qml_module(apptaquin_ui
    URI taquin_ui
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(apptaquin_ui PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.apptaquin_ui
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(apptaquin_ui
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Concurrent
)

install(TARGETS apptaquin_ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
