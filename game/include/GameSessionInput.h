#include "Arya.h"

class ClientGameSession;
class Unit;
using Arya::Rect;

class GameSessionInput : public Arya::InputListener, public Arya::FrameListener
{
    public:
        GameSessionInput(ClientGameSession* ses);
        virtual ~GameSessionInput();

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
		void setSpecPos(vec3 pos);

    private: 
        ClientGameSession* session;

        //For key movement
        bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown, rotatingLeft, rotatingRight;
        bool mouseLeft, mouseRight, mouseTop, mouseBot; //whether mouse is at edge
        bool draggingLeftMouse, draggingRightMouse;
		bool slowMode; //Precise movement

        //After clicking on "build house" or "cast skill" or "attack move"
        //you have to click on the map to complete this action.
        //The following bool specifies if we are in this state
        bool awaitingPlacement;

        bool leftShiftPressed;
		bool leftControlPressed;

        vec3 forceDirection;
        vec3 specMovement;
        vec3 specPos;

        vec2 originalMousePos;

        Rect* selectionRect;
        int waitWithUnitMovementNextFrames;
        int waitWithUnitSelectionNextFrames;
        int waitWithBuildingPlacementNextFrames;
        void selectUnit();

        void placeBuilding();
};
