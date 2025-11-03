cmake_minimum_required(VERSION 3.15)
project(MyProject VERSION 1.0 LANGUAGES CXX)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# find pthread / threads
find_package(Threads REQUIRED)

# include headers
include_directories(include)

# source files
file(GLOB SOURCES "src/*.cpp")

# create library from sources
add_library(MyProjectLib ${SOURCES})

# executable
add_executable(MyProject src/main.cpp)

target_compile_options(MyProject PRIVATE -O0)

# link library (not executable)
target_link_libraries(MyProject PRIVATE MyProjectLib)
target_link_libraries(MyProject PRIVATE Threads::Threads)

# optional: add tests
enable_testing()
add_subdirectory(tests)

