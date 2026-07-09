cmake_minimum_required(VERSION 3.20)

message(STATUS "Checking Toolchain...")
message(STATUS "CMAKE_TOOLCHAIN_FILE is set to: ${CMAKE_TOOLCHAIN_FILE}")

project(GameMathGym)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Is using MSVC? ${MSVC}")

# https://github.com/libsdl-org/SDL/releases/tag/release-3.4.2
find_package(SDL3 3.4.2 REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED) # will locate OpenGL installed on your system's operating system

message("Is SDL3 package found? ${SDL3_FOUND}")
message("Is imgui package found? ${imgui_FOUND}")
message("SDL3 version ${SDL3_VERSION}")
message("imgui version ${imgui_VERSION}")

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
    OpenGL::GL
    imgui::imgui
)

# web builds -- to generate an html file
if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

  target_link_options(${PROJECT_NAME} PRIVATE
    "-sUSE_GLFW=3" # for using OpenGL later
  )

endif()