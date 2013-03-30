#include "Arya.h"
#include "../include/GameSession.h"
#include "../include/Events.h"

using Arya::Root;
using Arya::Scene;
using Arya::Object;
using Arya::Model;
using Arya::ModelManager;
using Arya::Camera;
using Arya::Texture;
using Arya::TextureManager;
using Arya::SoundManager;
using Arya::Config;
using Arya::cvar;

class Network;
class EventManager;
class Scripting;

class Game : public Arya::InputListener , public Arya::FrameListener, public EventHandler, public Arya::CommandListener, public Arya::ButtonDelegate
{
    public:
        Game();
        virtual ~Game();

        void run();

        bool keyDown(int key, bool keyDown);
        bool mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y);
        bool mouseWheelMoved(int delta);
        bool mouseMoved(int x, int y, int dx, int dy);

        void onFrame(float elapsedTime);
        void handleEvent(Packet& packet);

        static Game& shared() { return (*singleton); }
        EventManager* getEventManager() const { return eventManager; }
        Scripting* getScripting() const { return scripting; }

        int getClientId() const { return clientId; }
		void buttonClicked(Arya::Button* sender);
	
    private:
        static Game* singleton;

		Arya::Window* menuWindow;

        Root* root;
        Scripting* scripting;
        Network* network;
        EventManager* eventManager;
        GameSession* session;
		bool handleCommand(string command);
		void createSessionDebug(int sessionHash);
		void joinSession(int sessionHash);

        int clientId;

        //We want to check the network
        //every 5 frames
        float timeSinceNetworkPoll;

		// menu and related functions
		bool initMenu();
		bool startLocalGame();
		bool startOnlineGame();
		bool startMapEditorSession();
		bool newSession(string serveraddr);
};
