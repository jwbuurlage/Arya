CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET(
	PROJECT_NAME
    "AryaGame"
)

SET(
	PROJECT_SOURCES
    "../src/Game.cpp"
    "../src/GameSession.cpp"
    "../src/ClientGameSession.cpp"
    "../src/GameSessionInput.cpp"
    "../src/Scripting.cpp"
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
	"../src/main.cpp"
)

SET(
	PROJECT_INCLUDES
	"../../include"
	"../include"
    "/usr/include/lua5.1"
)

IF(APPLE)
    SET(
		PROJECT_LIBRARIES
		"Arya"
		"PocoNet"
		"PocoFoundation"
		"pthread"
		"luabind"
        "lua"
	)
ELSE()
	SET(
		PROJECT_LIBRARIES
		"Arya"
		"PocoNet"
		"PocoFoundation"
		"pthread"
		"luabind"
        "lua5.1"
	)
ENDIF()

ADD_DEFINITIONS(-DPOCO_NO_AUTOMATIC_LIBS)
ADD_DEFINITIONS(-DPOCO_STATIC)

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )

