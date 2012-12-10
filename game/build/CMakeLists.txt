CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET(
	PROJECT_NAME
    "AryaGame"
)

SET(
	PROJECT_SOURCES
    "../src/Game.cpp"
    "../src/GameSession.cpp"
    "../src/Faction.cpp"
    "../src/Units.cpp"
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
)

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )

