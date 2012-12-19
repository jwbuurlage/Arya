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
    // - The clients his ID
    //------------------------
    EVENT_CLIENT_ID,

    EVENT_GAME_READY,
    EVENT_GAME_FULLSTATE,

    //------------------------
    // - Number of units NUM
    // - NUM Unit IDs
    // - NUM (vec2) positions
    //------------------------
    EVENT_MOVE_UNIT_REQUEST = 3000,

    //------------------------
    // - ...
    //------------------------
    EVENT_MOVE_UNIT,

    //------------------------
    // - Number of units
    // - Unit IDs
    // - Targetunit ID
    //------------------------
    EVENT_ATTACK_MOVE_UNIT_REQUEST,

    //------------------------
    // - ...
    //------------------------
    EVENT_ATTACK_MOVE_UNIT,
} EventCode;
