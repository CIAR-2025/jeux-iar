cmake_minimum_required(VERSION 3.16)

project(jeux_virus VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appjeux_virus
    main.cpp
    virus_core.cpp
    virus_ai.cpp
    virus_controller.cpp
)

qt_add_qml_module(appjeux_virus
    URI jeux_virus
    QML_FILES
        Main.qml
    RESOURCES
        assets/flip.wav
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appjeux_virus PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appjeux_virus
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appjeux_virus
    PRIVATE Qt6::Quick
            Qt6::Multimedia
)

install(TARGETS appjeux_virus
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --- Console version (requested by subject) ---
add_executable(virus_console
    main_console.cpp
    virus_core.cpp
    virus_ai.cpp
)

target_compile_features(virus_console PRIVATE cxx_std_17)
