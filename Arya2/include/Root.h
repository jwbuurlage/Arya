#pragma once

namespace Arya
{
    class World;
    class InputSystem;
    class Graphics;
    class Interface;

    class Root
    {
        public:
            Root();
            ~Root();

            //Parameters?
            //fullscreen, width, height
            bool init();

            void startGameLoop();
            void stopGameLoop();

            bool getFullscreen() const;
            void setFullscreen(bool fullscreen = true);

            int getWindowWidth() const;
            int getWindowHeight() const;

            World*       getWorld() const { return world; }
            Interface*   getInterface() const { return interface; }
            Graphics*    getGraphics() const { return graphics; }
            InputSystem* getInputSystem() const { return inputSystem; }

        private:
            World*       world;
            Interface*   interface;
            Graphics*    graphics;
            InputSystem* inputSystem;

    };
}
