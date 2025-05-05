cmake_minimum_required(VERSION 3.5)

project(Metin2Timer)

# Use automoc, autouic, and autorcc for Qt integration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#Set path for windows


#Set cmake that works with Qt
if(WIN32)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/msvc2022_64/lib/cmake")
ENDIF()

# Find Qt6 Widgets
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

# Define source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
)

# Define header files
set(HEADERS
    src/mainwindow.h
)

# Define UI files
set(UI_FILES
    src/mainwindow.ui
)

# Add Qt resources
qt6_add_resources(RESOURCES resources.qrc)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# Link Qt6 Widgets library
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Multimedia)

