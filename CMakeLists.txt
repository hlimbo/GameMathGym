cmake_minimum_required(VERSION 4.2)

project(GameMathGym)

message(STATUS "Checking Toolchain...")
message(STATUS "CMAKE_TOOLCHAIN_FILE is set to: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

find_package(OpenGL REQUIRED) # will locate OpenGL installed on your system's operating system
find_package(glad 0.1.36 REQUIRED)
# https://github.com/libsdl-org/SDL/releases/tag/release-3.4.2
find_package(SDL3 3.4.2 REQUIRED)
find_package(imgui CONFIG REQUIRED)

message(STATUS "Is SDL3 package found? ${SDL3_FOUND}")
message(STATUS "Is imgui package found? ${imgui_FOUND}")

message(STATUS "OpenGL Version ${OpenGL_VERSION}")
message(STATUS "glad Version ${glad_VERSION}")
message(STATUS "SDL3 version ${SDL3_VERSION}")
message(STATUS "imgui version ${imgui_VERSION}")

add_executable(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PRIVATE
    main.cpp
)

# this implicitly passes include paths and library paths for the game application
target_link_libraries(
  ${PROJECT_NAME}
    OpenGL::GL  
    glad::glad
    SDL3::SDL3
    imgui::imgui
)

# web builds -- to generate an html file
if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

  target_link_options(${PROJECT_NAME} PRIVATE
    "-sUSE_GLFW=3" # for using OpenGL later
  )

endif()

### DEBUG SETUP ###
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
      /W4                     # Level 4 warnings (roughly equivalent to -Wall -Wextra)
      /permissive-            # Enforces strict C++ standards conformance (like -Wpedantic)
      /w14242                 # Warns about hidden conversions (e.g., packing/conversion issues)
      /w14265                 # Warns if a class has virtual functions but a non-virtual destructor
      /w14287                 # Warns about unsigned/negative constant mismatches
      /w14311                 # Warns about pointer truncation (64-bit portability)
      /fsanitize=address      # Covers memory errors, out-of-bounds, use-after-free issues
    )
  endif()
endif()