cmake_minimum_required(VERSION 2.8)

# ALL OF THE SPECIFIED NAMES IN THIS CMAKE ARE REQUIRED.
# Set the project name variable to hw2.
# THIS IS REQUIRED
project(hw2)

# Add your CMAKE_C_FLAGS AND CMAKE_CXX_FLAGS here
# Make SURE to use -std=c11 and -std=c++11 where relevant.
# The options specified below are required, but additional options
#  may be used.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror")

# Add our include directory to CMake's search paths
# THIS IS REQUIRED
# include_directories -- Global (affects all targets)
# target_include_directories --	Local (only affects specific target)
# Which do we want here?
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


# Create library from dyn_array so we can use it later
# What are we doing with following statement? Creating a static library named dyn_array with source file dyn_array.c
add_library(dyn_array STATIC src/dyn_array.c)


# Compile the analysis executable
add_executable(analysis_test test/tests.cpp src/analysis.c)


# link the dyn_array library we compiled against our analysis executable
target_link_libraries(analysis_test PRIVATE dyn_array)


# Compile the tester executable
add_executable(${PROJECT_NAME}_test test/tests.cpp)

target_compile_definitions(${PROJECT_NAME}_test PRIVATE)

# Link ${PROJECT_NAME}_test with dyn_array and gtest and pthread libraries
target_link_libraries(${PROJECT_NAME}_test gtest pthread dyn_array)
