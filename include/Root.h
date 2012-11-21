namespace Arya
{
    class ShaderManager;

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

            ShaderManager* shaders;
    };
}
