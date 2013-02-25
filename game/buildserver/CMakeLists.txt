CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

ADD_DEFINITIONS(-DSERVERONLY)

SET(
	PROJECT_NAME
	"AryaServer"
)

SET(
	PROJECT_SOURCES
	"../../src/common/Logger.cpp"
	"../../src/Files.cpp"
    "../src/Map.cpp"
    "../src/Faction.cpp"
    "../src/Units.cpp"
    "../src/UnitTypes.cpp"
    "../src/Network.cpp"
    "../src/Server.cpp"
    "../src/Events.cpp"
    "../src/ServerClientHandler.cpp"
    "../src/ServerClient.cpp"
    "../src/ServerGameSession.cpp"
    "../src/common/GameLogger.cpp"
	"../src/servermain.cpp"
)

SET(
	PROJECT_INCLUDES
	"../../include"
	"../include"
)

SET(
	PROJECT_LIBRARIES
    "PocoNet"
    "PocoFoundation"
    "pthread"
)

ADD_DEFINITIONS(-DPOCO_NO_AUTOMATIC_LIBS)
ADD_DEFINITIONS(-DPOCO_STATIC)

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )

