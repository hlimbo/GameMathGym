set(PROGRAM_NAME "DrawTriangle")
add_executable(${PROGRAM_NAME})
target_sources(
  ${PROGRAM_NAME}
  PRIVATE
    draw_triangle.cpp
)

target_link_libraries(
  ${PROGRAM_NAME}
  PRIVATE
    OpenGL::GL
    glad::glad
    SDL3::SDL3
)

if(EMSCRIPTEN)
  set_target_properties(${PROGRAM_NAME} PROPERTIES SUFFIX ".html")
  # Target WebGL 2 for OpenGL 3.3 Core features support
  target_link_options(${PROGRAM_NAME} PRIVATE "-sMAX_WEBGL_VERSION=2")

  # Tell Emscripten to embed the "shaders" folder into virtual file system relative to where CMakeLists.txt is located
  # See: https://emscripten.org/docs/porting/files/packaging_files.html#modifying-file-locations-in-the-virtual-file-system
  target_link_options(${PROGRAM_NAME} PRIVATE "--embed-file" "${CMAKE_CURRENT_SOURCE_DIR}/../../shaders@/shaders")
endif()