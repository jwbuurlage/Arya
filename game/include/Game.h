#include "Arya.h"

using Arya::Root;
using Arya::Scene;
using Arya::Object;
using Arya::Model;
using Arya::ModelManager;
using Arya::Camera;
using Arya::Map;
using Arya::Texture;
using Arya::TextureManager;

class Game : public Arya::InputListener, public Arya::FrameListener
{
    public:
        Game();
        ~Game();

        void run();

        void onFrame(float elapsedTime);
        bool keyDown(int key, bool keyDown);
        bool mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y);
        bool mouseWheelMoved(int delta);
        bool mouseMoved(int x, int y, int dx, int dy);
    public:
        Root* root;

        //For key movement
        bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown, rotatingLeft, rotatingRight;
        bool mouseLeft, mouseRight, mouseTop, mouseBot; //wether mouse is at edge
        bool draggingLeftMouse, draggingRightMouse;
        vec3 forceDirection;
        vec3 specMovement;
        vec3 specPos;
};
