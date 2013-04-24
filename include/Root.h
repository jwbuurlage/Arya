#pragma once

#include <list>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include "common/Listeners.h"
#include "common/Singleton.h"

using glm::vec3;
using glm::mat4;

namespace Arya
{
    class Scene;
    class Overlay;
    class Interface;

    class Root : public Singleton<Root>, public CommandListener
    {
        public:
            Root();
            virtual ~Root();

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

            Overlay* getOverlay() const;
			mat4 getPixelToScreenTransform() const;

            vec3 getDepthResult(){ return clickScreenLocation; }
            void readDepth() { readDepthNextFrame = true; }

            float getAspectRatio() const { if(windowHeight == 0) return 0; return windowWidth/(float)windowHeight; }
        private:
            bool initGLFW();
            bool initGLEW();
            Scene* scene;
            Interface* interface;

            int windowWidth, windowHeight;
            int desktopWidth, desktopHeight;

            bool fullscreen;
            bool running;
            void render();

            double oldTime;

            //IMPORTANT: These have to be lists
            //instead of vectors because we need the
            //possibility to add and erase elements
            //while looping over them!
            std::list<FrameListener*> frameListeners;
            std::list<InputListener*> inputListeners;

            void windowSizeChanged(int width, int height);
            void keyDown(int key, int action);
            void mouseDown(int button, int action);
            void mouseWheelMoved(int pos);
            void mouseMoved(int x, int y);
            int mouseX, mouseY, mouseWheelPos; //To supply relative movement

            vec3 clickScreenLocation;
            bool readDepthNextFrame;

            friend void GLFWCALL windowSizeCallback(int width, int height);
            friend void GLFWCALL keyCallback(int key, int action);
            friend void GLFWCALL mouseButtonCallback(int button, int action);
            friend void GLFWCALL mousePosCallback(int x, int y);
            friend void GLFWCALL mouseWheelCallback(int pos);
    };
}
