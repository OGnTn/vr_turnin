cmake_minimum_required(VERSION 3.10)
#set debug
set(CMAKE_BUILD_TYPE Debug)

# Set project name
project(MyProject)
#specify source directory
#set binary directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#set library directory

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the paths to your source and include directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

# Add the include directories
include_directories(
    ${INCLUDE_DIR}/user
    ${INCLUDE_DIR}/
    ${INCLUDE_DIR}/glad
)

# Find all the source files
file(GLOB_RECURSE SOURCES "${INCLUDE_DIR}/*.cpp" "${SRC_DIR}/*.cpp" "${SRC_DIR}/glad.c")

# Find all the libraries
file(GLOB LIBRARIES "${LIB_DIR}/*.a")

# Create the executable
add_executable(MyProject ${SOURCES})
# Link the libraries, including Zlib
target_link_libraries(MyProject ${LIBRARIES} z)

# Set the library search path
link_directories(${LIB_DIR})
