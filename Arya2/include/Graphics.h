#pragma once

namespace Arya
{
    class Renderer;
    class Camera;
    class MaterialSystem;
    class TextureSystem;

    class Graphics
    {
        public:
            Graphics();
            ~Graphics();

            bool init();

            //! Clear the screen
            void clear(int width, int height);

            //TODO: World* or some list of GraphicsComponent as argument
            void render();

            Renderer*       getRenderer() const { return renderer; }
            Camera*         getCamera() const { return camera; }
            MaterialSystem* getMaterialSystem() const { return materialSystem; }
            TextureSystem*  getTextureSystem() const { return textureSystem; }

        private:
            Renderer*       renderer;
            Camera*         camera;
            MaterialSystem* materialSystem;
            TextureSystem*  textureSystem;
    };
}
