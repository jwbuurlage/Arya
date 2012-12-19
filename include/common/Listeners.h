#pragma once
#include <string>
using std::string;

namespace Arya
{
    typedef enum { //NOTE: These must the same as defined in GLFW
        BUTTON_LEFT = 0,
        BUTTON_RIGHT = 1,
        BUTTON_MIDDLE = 2} MOUSEBUTTON;

    //Subclass this to receive input
    class InputListener{
        public:
            //When returning false, next listener will be called. When returning true, the chain stops.
            virtual bool keyDown(int key, bool keyDown){ return false; }
            virtual bool mouseDown(MOUSEBUTTON button, bool buttonDown, int x, int y){ return false; }
            virtual bool mouseWheelMoved(int delta){ return false; }
            virtual bool mouseMoved(int x, int y, int dx, int dy){ return false; }
    };

    //Subclass to be called every frame
    class FrameListener{
        public:
            //Time is since previous frame
            virtual void onFrame(float elapsedTime) { return; }
            virtual void onRender() { return; }
    };

    class CommandListener
    {
        public:
            virtual bool onCommand(string command) {return false;}
    };
}
