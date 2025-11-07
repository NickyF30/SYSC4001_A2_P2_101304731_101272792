cmake_minimum_required(VERSION 4.0)
project(SYSC_4001_Assignment_2_Concurrent_Processes_Part_II)

set(CMAKE_CXX_STANDARD 26)

add_executable(SYSC_4001_Assignment_2_Concurrent_Processes_Part_II
        part1_parent.cpp
        part2_parent.cpp
        part2_child.cpp
        part3_parent.cpp
        part3_child.cpp
)
