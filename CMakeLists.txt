# Projects Settings
cmake_minimum_required (VERSION 3.2)
set (proj opengl-basic-engine)
project (${proj})

set(MAIN_SOURCES
  Application.cpp
  Application.hpp
  CollisionManager.cpp
  CollisionManager.hpp
  glError.cpp
  glError.hpp
  main.cpp
  MainApplication.cpp
  MainApplication.hpp
  Renderer.cpp
  Renderer.hpp
  Shader.cpp
  Shader.hpp
  StructInfo.hpp
)
list(TRANSFORM MAIN_SOURCES PREPEND "src/")

set(BUILDER_SOURCES
  Builder.hpp
  BoxBuilder.cpp
  BoxBuilder.hpp
  FrameBuilder.cpp
  FrameBuilder.hpp
  SphereBuilder.cpp
  SphereBuilder.hpp
  TetrahedronBuilder.cpp
  TetrahedronBuilder.hpp
  WFBoxBuilder.cpp
  WFBoxBuilder.hpp
  WFSphereBuilder.cpp
  WFSphereBuilder.hpp
)
list(TRANSFORM BUILDER_SOURCES PREPEND "src/builders/")

set(COMPONENT_SOURCES
  Box.cpp
  Box.hpp
  BoxCollider.cpp
  BoxCollider.hpp
  Camera.cpp
  Camera.hpp
  CollisionSolver.cpp
  CollisionSolver.hpp
  Component.cpp
  Component.hpp
  EmptyTransform.hpp
  Meshable.cpp
  Meshable.hpp
  Physical.cpp
  Physical.hpp
  Renderable.cpp
  Renderable.hpp
  RigidBody.cpp
  RigidBody.hpp
  Scene.cpp
  Scene.hpp
  Sphere.cpp
  Sphere.hpp
  SphereCollider.cpp
  SphereCollider.hpp
  Tetrahedron.cpp
  Tetrahedron.hpp
  TexturedMesh.cpp
  TexturedMesh.hpp
  World.cpp
  World.hpp
)
list(TRANSFORM COMPONENT_SOURCES PREPEND "src/components/")

set(SCENE_SOURCES
  BallpitScene.hpp
  BowlingScene.hpp
  BowlsScene.hpp
  DominoScene.hpp
  DynamicScene.hpp
  PoolScene.hpp
  SpinningBatScene.hpp
)
list(TRANSFORM SCENE_SOURCES PREPEND "src/scenes/")

set(VENDOR_SOURCES
  stb_image.h
)
list(TRANSFORM VENDOR_SOURCES PREPEND "src/vendors/")

# The main executable
add_executable(${proj}
  ${MAIN_SOURCES}
  ${BUILDER_SOURCES}
  ${COMPONENT_SOURCES}
  ${SCENE_SOURCES}
  ${VENDOR_SOURCES}
)

set_property(TARGET ${proj} PROPERTY CXX_STANDARD 20)
target_compile_options(${proj} PRIVATE -Wall)

add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindImguiTarget.cmake)
target_link_libraries(IMGUI PRIVATE glfw)

target_link_libraries(${proj}
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
  PRIVATE IMGUI
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)
target_include_directories(${proj}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)
