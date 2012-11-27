CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Arya)

SET(
  PROJECT_NAME
  "Arya"
  )

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

IF(APPLE)
  SET(
    CMAKE_CXX_LINK_FLAGS
    "-Wall -framework Cocoa -framework OpenGL -framework IOKit"
    )
ENDIF()

SET(
  PROJECT_SOURCES
  "../src/main.cpp"
  "../src/Root.cpp"
  "../src/Shaders.cpp"
  "../src/Objects.cpp"
  "../src/Scene.cpp"
  "../src/Files.cpp"
  "../src/Primitives.cpp"
  "../src/Mesh.cpp"
  "../src/Camera.cpp"
  "../src/common/Logger.cpp"
  )

SET(
  PROJECT_INCLUDES
  "../include"
  )

SET(
  PROJECT_LIBRARIES
  "GL"
  "GLU"
  "GLEW"
  "glfw"
  )

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )
