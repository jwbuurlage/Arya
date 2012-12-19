CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET(
	PROJECT_NAME
    "AryaGame"
)

SET(
	PROJECT_SOURCES
    "../src/Game.cpp"
    "../src/GameSession.cpp"
    "../src/GameSessionInput.cpp"
    "../src/Faction.cpp"
    "../src/Units.cpp"
    "../src/UnitTypes.cpp"
    "../src/Network.cpp"
    "../src/Server.cpp"
    "../src/Events.cpp"
    "../src/ServerClientHandler.cpp"
    "../src/ServerClient.cpp"
	"../src/main.cpp"
)

SET(
	PROJECT_INCLUDES
	"../../include"
	"../include"
)

SET(
	PROJECT_LIBRARIES
	"Arya"
    "PocoNet"
    "PocoFoundation"
)

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )

