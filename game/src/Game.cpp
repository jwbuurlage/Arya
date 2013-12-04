#include "../include/common/GameLogger.h"
#include "../include/Game.h"
#include "../include/ClientGameSession.h"
#include "../include/Network.h"
#include "../include/Events.h"
#include "../include/Scripting.h"
#include <GLFW/glfw3.h>
#include "Arya.h"

Game* Game::singleton = 0;

#define NETWORK_POLL 0.05f

Game::Game()
{
	root = 0;
	scripting = 0;
	session = 0;
	eventManager = 0;
	network = 0;
	clientId = 0;

	timeSinceNetworkPoll = 1.0f;
	menuWindow = 0;

	singleton = this;
}

Game::~Game()
{
	Arya::CommandHandler::shared().removeCommandListener(this);
    if(menuWindow) delete menuWindow;
	//Delete session before eventmanager, or else it will crash
	if(session) delete session;
	if(eventManager) delete eventManager;
	if(network) delete network;
	if(scripting) delete scripting;
	if(root) delete &Root::shared();

}

void Game::run()
{
	root = new Root;
	if(!(root->init(Config::shared().getCvarBool("fullscreen"), 800, 600)))
	{
		GAME_LOG_ERROR("Unable to init root");
	}
	else
	{
		root->addInputListener(this);

		scripting = new Scripting();
		scripting->init();

		scripting->execute("units.lua");
		scripting->execute("maps.lua");

		Arya::CommandHandler::shared().addCommandListener("createsession" ,this);
		Arya::CommandHandler::shared().addCommandListener("joinsession" ,this);

		if(network) delete network;
		network = new Network;

		if(eventManager) delete eventManager;
		eventManager = new EventManager(network);

		network->setPacketHandler(eventManager);

		eventManager->addEventHandler(EVENT_SESSION_INFO, this);
		eventManager->addEventHandler(EVENT_CLIENT_ID, this);

		initMenu();

		root->addFrameListener(this);
		root->startRendering();
	}
}

bool Game::initMenu()
{
	vec2 windowSize = vec2(300.0f, 200.0f);
	menuWindow = new Arya::Window(vec2(0.0f, 0.0f), windowSize * -0.5f, windowSize, 
			TextureManager::shared().getTexture("white"), Arya::WINDOW_DRAGGABLE, "Menu",
			vec4(0.1f, 0.1f, 0.1f, 1.0f));

	Arya::Button* localButton;
	Arya::Button* onlineButton;
	Arya::Button* mapEditButton;
	Arya::Button* quitButton;

	Arya::Font* f = Arya::FontManager::shared().getFont("DejaVuSans-Bold.ttf");
	Texture* t = TextureManager::shared().getTexture("white");

	localButton = new Arya::Button(
			vec2(-1.0, 1.0), vec2(10.0f, -60.0f), vec2(280.0f, 30.0f),
			t, f, "Local", "localButton",
		    this, false, vec4(0.5f, 0.5f, 0.5f, 1.0f)
			);
	menuWindow->addChild(localButton);

	onlineButton = new Arya::Button(
			vec2(-1.0, 1.0), vec2(10.0f, -100.0f), vec2(280.0f, 30.0f),
			t, f, "Online", "onlineButton",
		    this, false, vec4(0.5f, 0.5f, 0.5f, 1.0f)
			);
	menuWindow->addChild(onlineButton);

	mapEditButton = new Arya::Button(
			vec2(-1.0, 1.0), vec2(10.0f, -140.0f), vec2(280.0f, 30.0f),
			t, f, "Map Editor", "mapEditButton",
		    this, false, vec4(0.5f, 0.5f, 0.5f, 1.0f)
			);
	menuWindow->addChild(mapEditButton);

	quitButton = new Arya::Button(
			vec2(-1.0, 1.0), vec2(10.0f, -180.0f), vec2(280.0f, 30.0f),
			t, f, "Exit Game", "quitButton",
		    this, false, vec4(0.5f, 0.5f, 0.5f, 1.0f)
			);
	menuWindow->addChild(quitButton);

	Arya::Interface::shared().makeActive(menuWindow);

	return true;
}

void Game::buttonClicked(Arya::Button* sender)
{
	if(sender->getIdentifier() == "localButton")
		startLocalGame();
	else if(sender->getIdentifier() == "onlineButton")
		startOnlineGame();
	else if(sender->getIdentifier() == "mapEditButton")
		startMapEditorSession();
	else if(sender->getIdentifier() == "quitButton")
		Root::shared().stopRendering();
	
	Arya::Interface::shared().makeInactive(menuWindow);
}

bool Game::startLocalGame()
{
	network->startServer();
	network->connectToLobbyServer("localhost", 13337);
	newSession("localhost");
	return true;
}

bool Game::startOnlineGame()
{
	cvar* var = Config::shared().getCvar("serveraddress");
	const char* serveraddr = (var ? var->value.c_str() : "localhost");
	network->connectToLobbyServer(serveraddr, 13337);

	newSession(serveraddr);
	return true;
}

bool Game::startMapEditorSession()
{
	return true;
}

bool Game::newSession(string serveraddr)
{
	GAME_LOG_INFO("starting new session");

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
	network->connectToSessionServer(serveraddr, 13337);
	Event& joinEvent = eventManager->createEvent(EVENT_JOIN_GAME);
	joinEvent << 50505; //session hash
	joinEvent << Config::shared().getCvarInt("sessionhash"); //my secret hash
	joinEvent.send();
	//END TEMPORARY

	return true;
}

bool Game::keyDown(int key, bool keyDown)
{
	bool keyHandled = true;
	switch(key) {
		case 'P':
			if(keyDown) {
				if(session) delete session;
				session = new ClientGameSession;
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
		case 'O': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case 'I': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case GLFW_KEY_F11: Root::shared().setFullscreen(!Root::shared().getFullscreen()); break;
		case GLFW_KEY_ESCAPE: if(keyDown) Root::shared().stopRendering(); break;
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
		network->update();
        return;
	timeSinceNetworkPoll += elapsedTime;
	if(timeSinceNetworkPoll > NETWORK_POLL)
	{
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
			session = new ClientGameSession;

			if(!session->init()) {
                delete session;
                session = 0;
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
bool Game::handleCommand(string command)
{
	bool flag = true;
	if(Arya::CommandHandler::shared().splitLineCommand(command) == "createsession") 
	{
		int int1 = 0;
		std::stringstream parser;
		parser << Arya::CommandHandler::shared().splitLineParameters(command);
		parser >> int1;
		GAME_LOG_INFO("Creating session " << "...");
		createSessionDebug(int1);
		GAME_LOG_INFO("Session " << int1 <<  " created!");
	}
	else if(Arya::CommandHandler::shared().splitLineCommand(command) == "joinsession")
	{
		int int1 = 0;
		std::stringstream parser;
		parser << Arya::CommandHandler::shared().splitLineParameters(command);
		parser >> int1;
		GAME_LOG_INFO("Joing session " << int1 << "...");
		joinSession(int1);
		GAME_LOG_INFO("Session " << int1 << " joined!");
	}
	else flag = false;
	return flag;
}

void Game::createSessionDebug(int sessionHash)
{
	//TEMPORARY:
	//we assume the client has told the lobby server to start the game (EVENT_SESSION_START)
	//that means the lobby server has to tell the game server to create the actual game
	//and then the lobby server should return the gameserver-ip to this client
	//currently we act like THIS CLIENT is the lobby server
	//telling the game server to create a new session
	//and we act like we already received the EVENT_SESSION_INFO from the lobby server
	Event& lobbyToGameEvent = eventManager->createEvent(EVENT_NEW_SESSION);
	lobbyToGameEvent << sessionHash; //session hash
	lobbyToGameEvent << 4; //player count
	lobbyToGameEvent << 10101; //secret hashes for each player
	lobbyToGameEvent << 10102;
	lobbyToGameEvent << 10103;
	lobbyToGameEvent << 10104;
	lobbyToGameEvent.send();
}
void Game::joinSession(int sessionHash)
{
	if(session) delete session;
	session = 0;

	//Reconnect
	cvar* var = Config::shared().getCvar("serveraddress");
	const char* serveraddr = (var ? var->value.c_str() : "localhost");
	network->connectToSessionServer(serveraddr, 13337);

	Event& joinEvent = eventManager->createEvent(EVENT_JOIN_GAME);
	joinEvent << sessionHash; //session hash
	joinEvent << Config::shared().getCvarInt("sessionhash"); //my secret hash
	joinEvent.send();
}
