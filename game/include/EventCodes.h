#pragma once

typedef enum
{
    // Lobby
    //
    //account login
    //ranking queries
    //room queries, room joins
    //chats
    //hacks
    //god mode

    // Session
    EVENT_JOIN_GAME = 2001,

    //------------------------
    // - The client his ID
    //------------------------
    EVENT_CLIENT_ID,

    EVENT_GAME_READY,
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
	EVENT_UNIT_DIED
} EventCode;
