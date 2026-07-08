cmake_minimum_required(VERSION 3.20)

message(STATUS "Checking Toolchain...")
message(STATUS "CMAKE_TOOLCHAIN_FILE is set to: ${CMAKE_TOOLCHAIN_FILE}")

project(GameMathGym)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Is using MSVC? ${MSVC}")

# https://github.com/libsdl-org/SDL/releases/tag/release-3.4.2
find_package(SDL3 3.4.2 REQUIRED)

message("Is SDL3 package found? ${SDL3_FOUND}")

add_executable(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PRIVATE
    main.cpp
)

# this implicitly passes include paths and library paths for the game application
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    SDL3::SDL3
)