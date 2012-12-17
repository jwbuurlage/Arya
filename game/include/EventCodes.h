typedef enum
{
    // Lobby
    //
    //account login
    //ranking queries
    //room queries, room joins
    //chats

    // Session
    EVENT_JOIN_GAME = 2001,
    EVENT_CLIENT_ID, //server sends client his client id in response to JOIN_GAME
    EVENT_GAME_READY,
    EVENT_GAME_FULLSTATE,

    EVENT_MOVE_UNIT = 2001
} EventCode;
