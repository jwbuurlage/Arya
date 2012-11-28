#pragma once

#include <vector>
#include <GL/glfw.h>
#include "common/Singleton.h"

namespace Arya
{
    class Scene;

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
            virtual void onFrame(float elapsedTime){ return; }
    };

    class Root : public Singleton<Root>
    {
        public:
            Root();
            ~Root();

            bool init(bool fullscreen = false);
            void startRendering();
            void stopRendering();
            bool getFullscreen() const { return fullscreen; }
            void setFullscreen(bool fullscreen = true);

            //TODO: Extra arguments like APPEND_LAST or FRONT or CALL_ALWAYS or something??
            void addInputListener(InputListener* listener);
            void removeInputListener(InputListener* listener);

            void addFrameListener(FrameListener* listener);
            void removeFrameListener(FrameListener* listener);

            Scene* getScene() const { return scene; }

        private:
            bool initGLFW();
            bool initGLEW();
            Scene* scene;

            int desktopWidth, desktopHeight;

            bool fullscreen;
            bool running;
            void render();

            std::vector<FrameListener*> frameListeners;
            std::vector<InputListener*> inputListeners;
            void keyDown(int key, int action);
            void mouseDown(int button, int action);
            void mouseWheelMoved(int pos);
            void mouseMoved(int x, int y);
            int mouseX, mouseY, mouseWheelPos; //To supply relative movement

            friend void GLFWCALL keyCallback(int key, int action);
            friend void GLFWCALL mouseButtonCallback(int button, int action);
            friend void GLFWCALL mousePosCallback(int x, int y);
            friend void GLFWCALL mouseWheelCallback(int pos);
    };
}
