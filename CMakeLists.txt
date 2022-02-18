cmake_minimum_required(VERSION 3.14)

# set the project name
project(Parse_NOCI)

# Designate the include directories
include_directories(include)

# Use Glob for cxx sources
file(GLOB SOURCES src/*.cxx)

# Completed executable
add_executable(parseNOCI ${SOURCES})
target_link_libraries(parseNOCI ${PROJECT_LINK_LIBS})
