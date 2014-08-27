CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Arya)

#-- Where to search for the Arya lib
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/..bin/)

#-- Where to output the Arya lib and the game
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

#-- Display warnings and compile with debug information and optimize
SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall -Wextra -g -O3")

IF(APPLE)
    SET(
        LIB_LIBRARIES
        "GLEW"
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
        "SDL2"
        )
ENDIF()

SET(LIB_NAME "Arya")

SET(
    LIB_SOURCES
    "../src/common/Logger.cpp"
    "../src/Camera.cpp"
    "../src/Entity.cpp"
    "../src/EntitySystem.cpp"
    "../src/Graphics.cpp"
    "../src/InputSystem.cpp"
    "../src/Interface.cpp"
    "../src/Renderer.cpp"
    "../src/Root.cpp"
    "../src/Terrain.cpp"
    "../src/World.cpp"
    )

SET(
    LIB_INCLUDES
    "../include"
    )

ADD_DEFINITIONS(-DGLEW_STATIC)

INCLUDE_DIRECTORIES( ${LIB_INCLUDES} )
ADD_LIBRARY( ${LIB_NAME} STATIC ${LIB_SOURCES} )
TARGET_LINK_LIBRARIES( ${LIB_NAME} ${LIB_LIBRARIES} )

ADD_SUBDIRECTORY(../game/build ../game/build)
ADD_SUBDIRECTORY(../tools/build ../tools/build)
