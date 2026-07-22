  set(PROGRAM_NAME "ImguiDemo")
  add_executable(${PROGRAM_NAME})
  target_sources(
    ${PROGRAM_NAME}
    PRIVATE
      imgui_demo.cpp
  )

  # this implicitly passes include paths and library paths for the game application
  target_link_libraries(
    ${PROGRAM_NAME}
    PRIVATE
      OpenGL::GL  
      glad::glad
      SDL3::SDL3
      imgui::imgui
  )

  # web builds -- to generate an html file
  if(EMSCRIPTEN)
    set_target_properties(${PROGRAM_NAME} PROPERTIES SUFFIX ".html")

    # Target WebGL 2 for OpenGL 3.3 Core features support
    target_link_options(${PROGRAM_NAME} PRIVATE "-sMAX_WEBGL_VERSION=2")
  endif()