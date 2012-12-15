#include "Arya.h"
#include "../include/GameSession.h"

using Arya::Root;
using Arya::Scene;
using Arya::Object;
using Arya::Model;
using Arya::ModelManager;
using Arya::Camera;
using Arya::Map;
using Arya::Texture;
using Arya::TextureManager;

class Network;

class Game : public Arya::InputListener
{
    public:
        Game();
        ~Game();

        void run();

        bool keyDown(int key, bool keyDown);
        bool mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y);
        bool mouseWheelMoved(int delta);
        bool mouseMoved(int x, int y, int dx, int dy);

    private:
        Root* root;
        Network* network;
        GameSession* session;
};
