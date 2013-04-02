#pragma once

typedef enum
{
    // Lobby
	
	// - name
	// - password md5
	EVENT_LOGIN = 1001,

	// - int: account id
	EVENT_LOGIN_SUCCESS, //server->client
	
	// - string: reason
	EVENT_LOGIN_FAILED, //server->client

	EVENT_JOIN_LOBBY = 1001,

    //ranking queries
    //room queries, room joins
    //chats
    //hacks
    //god mode


	// - string: game server ip
	// - int: game server port
	// - int: session hash
	// - int: client hash
	EVENT_SESSION_INFO, //server->client

	//TODO: ready-up system and so on
	EVENT_SESSION_START, //client->server
	
	//
	// All packets above are between LOBBY SERVER and client
	//

	// The following events are between LOBBY SERVER and GAME SERVER
	
	// - int roomid or sessionhash
	// - int playercount
	//		- int client hash for each player
	// map info etc
	EVENT_NEW_SESSION, //lobby->game

	//
	// All packets below are between GAME SERVER and client
	//
	
	//EventManager uses this number to see if packets are meant for lobby or game server
	MARKER_MINIMUM_GAME_PACKET_ID = 2000,

	// - int roomid or sessionhash
	// - int client hash
    EVENT_JOIN_GAME, //client->server

	EVENT_CLIENT_READY, //client->server

	//some timer sync related things
	EVENT_GAME_START, //Game timer starts. server->client

    //------------------------
    // - The client his ID
    //------------------------
    EVENT_CLIENT_ID,

	EVENT_GAME_FULLSTATE_REQUEST,
    EVENT_GAME_FULLSTATE,

    // - clientID
    // - Serialized faction
    //    + UnitCount
    //    - Serialized unit 
    EVENT_CLIENT_CONNECTED,

    // - Client ID
    EVENT_CLIENT_DISCONNECTED,

    //------------------------
    // - Number of units NUM
    // - NUM Unit IDs
    // - NUM (vec2) positions
    //------------------------
    EVENT_MOVE_UNIT_REQUEST = 3000,

    //------------------------
    // - Number of units NUM
    //   - Unit ID
    //   - (vec2) position
    //------------------------
    EVENT_MOVE_UNIT,

    //------------------------
    // - Targetunit ID
    // - Number of units
    // - Unit IDs
    //------------------------
    EVENT_ATTACK_MOVE_UNIT_REQUEST,

    //------------------------
    // - Targetunit ID
    // - Number of units
    // - Unit IDs
    //------------------------
    EVENT_ATTACK_MOVE_UNIT,

	//------------------------
	// - Unit ID
	//------------------------
	EVENT_UNIT_DIED,

    // - Faction ID
    // - Unit ID
    // - serialized unit
    EVENT_UNIT_SPAWNED,


	//------------------------
	// - Faction ID
	//------------------------
	EVENT_PLAYER_DEFEAT,

	//------------------------
	// - Faction ID
	//------------------------
	EVENT_PLAYER_VICTORY
} EventCode;
