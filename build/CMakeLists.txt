CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Arya)

#-- Where to search for the Arya lib
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/..bin/)
#-- Where to output the Arya lib and the game

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#-- Display warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

IF(APPLE)
    SET(
        LIB_LIBRARIES
        "GLEW"
        "glfw"
        ) 
    SET(
        CMAKE_CXX_LINK_FLAGS
        "-Wall -framework Cocoa -framework OpenGL -framework IOKit"
        )
ELSE()
    SET(
        LIB_LIBRARIES
        "GL"
        "GLEW"
        "glfw"
        )
ENDIF()

SET(LIB_NAME "Arya")

SET(
    LIB_SOURCES
    "../src/Root.cpp"
    "../src/Shaders.cpp"
    "../src/Models.cpp"
    "../src/Objects.cpp"
    "../src/Scene.cpp"
    "../src/Files.cpp"
    "../src/Primitives.cpp"
    "../src/Mesh.cpp"
    "../src/Camera.cpp"
    "../src/Terrain.cpp"
    "../src/Textures.cpp"
    "../src/Fonts.cpp"
    "../src/Map.cpp"
    "../src/Overlay.cpp"
    "../src/Interface.cpp"
    "../src/Materials.cpp"
    "../src/common/Logger.cpp"
    "../src/Console.cpp"
    )

SET(
    LIB_INCLUDES
    "../include"
    "../ext"
    )

INCLUDE_DIRECTORIES( ${LIB_INCLUDES} )
ADD_LIBRARY( ${LIB_NAME} STATIC ${LIB_SOURCES} )
TARGET_LINK_LIBRARIES( ${LIB_NAME} ${LIB_LIBRARIES} )

ADD_SUBDIRECTORY(../game/build ../game/build)
ADD_SUBDIRECTORY(../tools/build ../tools/build)
