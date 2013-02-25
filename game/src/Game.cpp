#include "../include/common/GameLogger.h"
#include "../include/Game.h"
#include "../include/GameSession.h"
#include "../include/Network.h"
#include "../include/Events.h"

Game* Game::singleton = 0;

#define NETWORK_POLL 0.05f

Game::Game()
{
    root = 0;
    session = 0;
    eventManager = 0;
    network = 0;
    clientId = 0;

    timeSinceNetworkPoll = 1.0f;

    singleton = this;
}

Game::~Game()
{
    if(eventManager) delete eventManager;
    if(network) delete network;
    if(session) delete session;
    if(root) delete &Root::shared();
}

void Game::run()
{
    root = new Root;

    if(!(root->init(true, 800, 600))) {
        GAME_LOG_ERROR("Unable to init root");
    }
    else
    {
        root->addInputListener(this);

        if(network) delete network;
        network = new Network;

        network->connectToLobbyServer("balubu.com", 13337);

        if(eventManager) delete eventManager;
        eventManager = new EventManager(network);

        network->setPacketHandler(eventManager);

		//!!!!!
		//TEMPORARY:
		//we assume the client has told the lobby server to start the game (EVENT_SESSION_START)
		//that means the lobby server has to tell the game server to create the actual game
		//and then the lobby server should return the gameserver-ip to this client
		//currently we act like THIS CLIENT is the lobby server
		//telling the game server to create a new session
		//and we act like we already received the EVENT_SESSION_INFO from the lobby server
        Event& lobbyToGameEvent = eventManager->createEvent(EVENT_NEW_SESSION);
		lobbyToGameEvent << 50505; //session hash
        lobbyToGameEvent << 4; //player count
		lobbyToGameEvent << 10101; //secret hashes for each player
		lobbyToGameEvent << 10102;
		lobbyToGameEvent << 10103;
		lobbyToGameEvent << 10104;
        lobbyToGameEvent.send();
		//in a normal scenario the following is done below in the eventhandler at EVENT_SESSION_INFO
		network->connectToSessionServer("balubu.com", 13337);
		Event& joinEvent = eventManager->createEvent(EVENT_JOIN_GAME);
		joinEvent << 50505; //session hash
		joinEvent << 10102; //my secret hash
		joinEvent.send();
		//END TEMPORARY

		eventManager->addEventHandler(EVENT_SESSION_INFO, this);
        eventManager->addEventHandler(EVENT_CLIENT_ID, this);

        root->addFrameListener(this);
        root->startRendering();
    }
}

bool Game::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;
    switch(key) {
        case 'P':
            if(keyDown) {
                if(session) delete session;
                session = new GameSession;
                if(!session->init()) {
                    GAME_LOG_ERROR("Could not start a new session");
                    Root::shared().stopRendering();
                }
            }
            break;

        case 'L':
            if(keyDown)
            {
                if(session) delete session;
                session = 0;
            }
            break;

		case '8':
			if(keyDown)
			{
				Event& ev = getEventManager()->createEvent(EVENT_GAME_FULLSTATE_REQUEST);
				ev.send();
			}
			break;

        case 'O': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        case 'I': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case GLFW_KEY_F11: Root::shared().setFullscreen(!Root::shared().getFullscreen()); break;
        case GLFW_KEY_ESC: if(keyDown) Root::shared().stopRendering(); break;
        default: keyHandled = false; break;
    }
    return keyHandled;
}

bool Game::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    return false;
}

bool Game::mouseWheelMoved(int delta)
{
    return false;
}

bool Game::mouseMoved(int x, int y, int dx, int dy)
{
    return false; 
}

void Game::onFrame(float elapsedTime)
{
    timeSinceNetworkPoll += elapsedTime;
    if(timeSinceNetworkPoll > NETWORK_POLL)
    {
        network->update();
        timeSinceNetworkPoll = 0.0f;
    }
}

void Game::handleEvent(Packet& packet)
{
    int id = packet.getId();
    switch(id)
    {
        case EVENT_CLIENT_ID:
            packet >> clientId;
			//After receiving our client ID we start the session. The server will soon send us the full game state
			if(session) delete session;
			session = new GameSession;

			if(!session->init()) {
				GAME_LOG_ERROR("Could not start a new session");
				Root::shared().stopRendering();
			}
            break;

		case EVENT_SESSION_INFO:
			GAME_LOG_WARNING("EVENT_SESSION_INFO is not supported yet. please upgrade your arya's and try again");
			break;

        default:
            GAME_LOG_INFO("Game: unknown event received! (" << id << ")");
            break;
    }
}
