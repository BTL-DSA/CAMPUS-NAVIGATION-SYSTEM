cmake_minimum_required(VERSION 3.15)
project(CampusNavCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(CampusNavLib STATIC
    src/Graph.cpp
    src/MinHeap.cpp
)

add_executable(campus_nav
    src/main.cpp
)

target_link_libraries(campus_nav PRIVATE CampusNavLib)

