#include "Arya.h"

using Arya::Root;

class Game : public Arya::InputListener, public Arya::FrameListener
{
    public:
        Game();
        ~Game();

        void run();

        void onFrame(float elapsedTime);
        bool keyDown(int key, bool keyDown);
    public:
        Root* root;

        //For key movement
        bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown, rotatingLeft, rotatingRight;
        vec3 forceDirection;
        vec3 specPos;
};
