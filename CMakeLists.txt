cmake_minimum_required(VERSION 3.15)
project(ChatRoom VERSION 1.0 LANGUAGES CXX)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# find pthread / threads
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQ REQUIRED libpq)

# include headers
include_directories(include)
include_directories(/usr/include/postgresql)

# source files
file(GLOB SERVER src/server/*.cpp
		 src/db/*.cpp
         src/ipc_server.cpp)

# create library from sources
add_library(ChatRoomLib ${SERVER})

# executable
add_executable(ChatRoom ${SERVER})

target_compile_options(ChatRoom PRIVATE -O0)

# link library (not executable)
target_link_libraries(ChatRoom PRIVATE Threads::Threads)
target_link_libraries(ChatRoom PRIVATE  ${LIBPQ_LIBRARIES})

# optional: add tests
enable_testing()
add_subdirectory(tests)

