cmake_minimum_required(VERSION 4.2)

project(GameMathGym)

message(STATUS "Checking Toolchain...")
message(STATUS "CMAKE_TOOLCHAIN_FILE is set to: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")


string(FIND "${VCPKG_MANIFEST_FEATURES}" "demos" DEMOS_FEATURE_INDEX)
if (DEMOS_FEATURE_INDEX GREATER -1)
  find_package(OpenGL REQUIRED) # will locate OpenGL installed on your system's operating system
  find_package(glad 0.1.36 REQUIRED)
  # https://github.com/libsdl-org/SDL/releases/tag/release-3.4.2
  find_package(SDL3 3.4.2 REQUIRED)
  find_package(imgui CONFIG REQUIRED)
  find_package(assimp CONFIG REQUIRED)
  find_package(Stb REQUIRED)

  message(STATUS "Is SDL3 package found? ${SDL3_FOUND}")
  message(STATUS "Is imgui package found? ${imgui_FOUND}")
  message(STATUS "Is stb package found? ${Stb_FOUND}")

  message(STATUS "OpenGL Version ${OpenGL_VERSION}")
  message(STATUS "glad Version ${glad_VERSION}")
  message(STATUS "SDL3 version ${SDL3_VERSION}")
  message(STATUS "imgui version ${imgui_VERSION}")
  message(STATUS "assimp version ${assimp_VERSION}")
  message(STATUS "Stb Include directory ${Stb_INCLUDE_DIR}")

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
      OpenGL::GL  
      glad::glad
      SDL3::SDL3
      imgui::imgui
  )

  add_executable("DrawTriangle")
  target_sources(
    "DrawTriangle"
    PRIVATE
      draw_triangle.cpp
  )

  target_link_libraries(
    "DrawTriangle"
    PRIVATE
      OpenGL::GL
      glad::glad
      SDL3::SDL3
  )

  add_executable("3DModelDemo")
  target_sources(
    "3DModelDemo"
    PRIVATE
      3d_model_demo.cpp
      "utils/shader_utils.cpp"
  )

  target_include_directories("3DModelDemo" PRIVATE ${Stb_INCLUDE_DIR})
  target_link_libraries(
    "3DModelDemo"
    PRIVATE
      OpenGL::GL
      glad::glad
      SDL3::SDL3
      assimp::assimp
  )

  add_executable("TexturesDemo")
  target_sources(
    "TexturesDemo"
    PRIVATE
      textures_demo.cpp
      "utils/shader_utils.cpp"
  )

  target_include_directories("TexturesDemo" PRIVATE ${Stb_INCLUDE_DIR})
  target_link_libraries(
    "TexturesDemo"
    PRIVATE
      OpenGL::GL
      glad::glad
      SDL3::SDL3
  )

  # web builds -- to generate an html file
  if(EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    set_target_properties("DrawTriangle" PROPERTIES SUFFIX ".html")

    # Target WebGL 2 for OpenGL 3.3 Core features support
    target_link_options(${PROJECT_NAME} PRIVATE "-sMAX_WEBGL_VERSION=2")
    target_link_options("DrawTriangle" PRIVATE "-sMAX_WEBGL_VERSION=2")

    # Tell Emscripten to embed the "shaders" folder into virtual file system relative to where CMakeLists.txt is located
    # See: https://emscripten.org/docs/porting/files/packaging_files.html#modifying-file-locations-in-the-virtual-file-system
    target_link_options("DrawTriangle" PRIVATE "--embed-file" "${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders")

  endif()
endif()

string (FIND "${VCPKG_MANIFEST_FEATURES}" "unit-tests" UNIT_TEST_FEATURE_INDEX)
if (UNIT_TEST_FEATURE_INDEX GREATER -1)
  # Enables testing for current directory and subdirectory -- enables capability to run CTest
  enable_testing()
  find_package(GTest CONFIG REQUIRED)
  message(STATUS "GTest Version: ${GTest_VERSION}")
  message(STATUS "Is GTest package found? ${GTest_FOUND}")

  add_executable("GTestSample")
  target_sources(
    "GTestSample"
    PRIVATE
      gtest_sample.cpp
  )
  target_link_libraries(
    "GTestSample" 
    PRIVATE
      GTest::gtest
      GTest::gtest_main
      GTest::gmock
      GTest::gmock_main
  )

  # Registers a suite of tests to run
  add_test(AllTestsInGTestSample "GTestSample")

  # Discovers and finds all tests suites in the target GTestSample
  # include(GoogleTest)
  # gtest_discover_tests("GTestSample")

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