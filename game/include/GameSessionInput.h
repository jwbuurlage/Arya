#include "Arya.h"

class GameSession;
using Arya::Rect;

class GameSessionInput : public Arya::InputListener, public Arya::FrameListener
{
    public:
        GameSessionInput(GameSession* ses);
        ~GameSessionInput();

        void init();

        void onFrame(float elapsedTime);
        bool keyDown(int key, bool keyDown);
        bool mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y);
        bool mouseWheelMoved(int delta);
        bool mouseMoved(int x, int y, int dx, int dy);

        void unselectAll();
        void selectAll();
        void selectUnits(float x_min, float x_max, float y_min, float y_max);

        void moveSelectedUnits();

    private: 
        GameSession* session;

        //For key movement
        bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown, rotatingLeft, rotatingRight;
        bool mouseLeft, mouseRight, mouseTop, mouseBot; //wether mouse is at edge
        bool draggingLeftMouse, draggingRightMouse;
		bool slowMode; //Precise movement

        bool leftShiftPressed;

        vec3 forceDirection;
        vec3 specMovement;
        vec3 specPos;

        vec2 originalMousePos;

        Rect selectionRect;
        bool doUnitMovementNextFrame;
};
