cmake_minimum_required(VERSION 3.10)

message(STATUS "Checking Toolchain...")
message(STATUS "CMAKE_TOOLCHAIN_FILE is set to: ${CMAKE_TOOLCHAIN_FILE}")

project(GameMathGym)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Is using MSVC? ${MSVC}")

