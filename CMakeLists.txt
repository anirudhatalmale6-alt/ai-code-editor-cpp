cmake_minimum_required(VERSION 3.16)
project(AICodeEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/codeeditor.cpp
    src/syntaxhighlighter.cpp
    src/aichatpanel.cpp
    src/aiservice.cpp
    src/compilerservice.cpp
)

set(HEADERS
    src/mainwindow.h
    src/codeeditor.h
    src/syntaxhighlighter.h
    src/aichatpanel.h
    src/aiservice.h
    src/compilerservice.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
