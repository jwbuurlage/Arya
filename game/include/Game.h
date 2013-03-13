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

class Game : public Arya::InputListener , public Arya::FrameListener, public EventHandler, public Arya::CommandListener
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
        EventManager* getEventManager() { return eventManager; }

        int getClientId() const { return clientId; }

    private:
        static Game* singleton;

        Root* root;
        Network* network;
        EventManager* eventManager;
        GameSession* session;

        int clientId;

        //We want to check the network
        //every 5 frames
        float timeSinceNetworkPoll;
};
