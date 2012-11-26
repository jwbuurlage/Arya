#pragma once


namespace Arya
{
    class Scene;

    class Root
    {
        public:
            Root() { };
            ~Root() { };

            bool init();
            void render();

        private:
            bool initGLFW();
            bool initGLEW();
            bool initShaders();
            bool initObjects();
            Scene* scene;
    };
}
