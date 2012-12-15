#pragma once

#include <vector>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include "common/Listeners.h"
#include "common/Singleton.h"

using glm::vec3;

namespace Arya
{
    class Scene;
    class Overlay;

    class Root : public Singleton<Root>
    {
        public:
            Root();
            ~Root();

            bool init(bool fullscreen = true, int width = 0, int height = 0);
            void startRendering();
            void stopRendering();
            bool getFullscreen() const { return fullscreen; }
            void setFullscreen(bool fullscreen = true);

            //TODO: Extra arguments like APPEND_LAST or FRONT or CALL_ALWAYS or something??
            void addInputListener(InputListener* listener);
            void removeInputListener(InputListener* listener);

            void addFrameListener(FrameListener* listener);
            void removeFrameListener(FrameListener* listener);

            Scene* makeDefaultScene();
            Scene* getScene() const { return scene; }
            void removeScene();

            int getWindowWidth() const { return windowWidth; }
            int getWindowHeight() const { return windowHeight; }

            Overlay* getOverlay() const { return overlay; }

            void readDepthNextFrame(int x, int y){ readDepthBuffer = true; readAtX = x; readAtY = y; }
            vec3 getDepthResult(){ return clickScreenLocation; }

        private:
            bool initGLFW();
            bool initGLEW();
            Scene* scene;
            Overlay* overlay;

            int windowWidth, windowHeight;
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

            bool readDepthBuffer;
            int readAtX;
            int readAtY;
            vec3 clickScreenLocation;

            friend void GLFWCALL keyCallback(int key, int action);
            friend void GLFWCALL mouseButtonCallback(int button, int action);
            friend void GLFWCALL mousePosCallback(int x, int y);
            friend void GLFWCALL mouseWheelCallback(int pos);
    };
}
