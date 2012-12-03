#pragma once

#include <vector>
#include <GL/glfw.h>
#include "common/Listeners.h"
#include "common/Singleton.h"

namespace Arya
{
    class Scene;

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

            double oldTime;

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
