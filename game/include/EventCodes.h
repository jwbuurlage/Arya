#pragma once

typedef enum
{
    //
    // Lobby <--> Client
    //
	
	// - name
	// - password md5
	EVENT_LOGIN = 1001,

	// - int: account id
	EVENT_LOGIN_SUCCESS, //server->client
	
	// - string: reason
	EVENT_LOGIN_FAILED, //server->client

	EVENT_JOIN_LOBBY,

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
	// Lobby <--> Game Server
	//
	
	// - int roomid or sessionhash
	// - int playercount
	//		- int client hash for each player
	// map info etc
	EVENT_NEW_SESSION, //lobby->game

	//
	// Game Server <--> Client
	//
	
	//EventManager uses this number to see if packets are meant for lobby or game server
	MARKER_MINIMUM_GAME_PACKET_ID = 2000,

    EVENT_PING,

    EVENT_JOIN_GAME, //client->server
	EVENT_CLIENT_READY, //client->server
	EVENT_GAME_START, //Game timer starts. server->client
    EVENT_CLIENT_ID,

	EVENT_GAME_FULLSTATE_REQUEST,
    EVENT_GAME_FULLSTATE,
    EVENT_CLIENT_CONNECTED,
    EVENT_CLIENT_DISCONNECTED,

    EVENT_MOVE_UNIT_REQUEST,
    EVENT_MOVE_UNIT,
    EVENT_ATTACK_MOVE_UNIT_REQUEST,
    EVENT_ATTACK_MOVE_UNIT,
	EVENT_UNIT_DIED,
    EVENT_UNIT_SPAWNED,
	EVENT_PLAYER_DEFEAT,
	EVENT_PLAYER_VICTORY,
    EVENT_SPAWN_REQUEST

} EventCode;
