cmake_minimum_required(VERSION 3.21)
project(SystemsSoftware-P3)

set(CMAKE_CXX_STANDARD 11)


add_executable(
        P3 main.cpp
        opcodes.cpp
        symbols.cpp
        errors.cpp
        directives.cpp
)
