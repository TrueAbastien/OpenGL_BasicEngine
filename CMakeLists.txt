# Projects Settings
cmake_minimum_required (VERSION 3.2)
set (proj opengl-basic-engine)
project (${proj})

# The main executable
add_executable(${proj}
  src/Application.hpp
  src/Application.cpp
  src/PendulumApplication.hpp
  src/PendulumApplication.cpp
  src/glError.hpp
  src/glError.cpp
  src/main.cpp
  src/Shader.hpp
  src/Shader.cpp
  src/Renderer.hpp
  src/Renderer.cpp
  src/Component.hpp
  src/Component.cpp
)

set_property(TARGET ${proj} PROPERTY CXX_STANDARD 20)
target_compile_options(${proj} PRIVATE -Wall)

add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)

target_link_libraries(${proj}
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)
target_include_directories(${proj}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)
