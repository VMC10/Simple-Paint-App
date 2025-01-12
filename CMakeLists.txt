cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Paint VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find wxWidgets package
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include wxWidgets
include(${wxWidgets_USE_FILE})

# If wxWidgets is not found, specify the path manually
# set(wxWidgets_ROOT_DIR "/path/to/wxWidgets")
# find_package(wxWidgets REQUIRED COMPONENTS core base)

# Add source files
set(SOURCES
    main.cpp
    paint.cpp
    frame.cpp
)

# Add header files
set(HEADERS
    path.h
    paint.h
    frame.h
    enum.h
)

# Add executable
add_executable(Paint ${SOURCES} ${HEADERS})

# Link wxWidgets libraries
target_link_libraries(Paint ${wxWidgets_LIBRARIES})

# Include directories for wxWidgets
target_include_directories(Paint PRIVATE ${wxWidgets_INCLUDE_DIRS})
