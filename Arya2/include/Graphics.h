#pragma once

namespace Arya
{
    class Renderer;
    class MaterialSystem;
    class TextureSystem;

    class Graphics
    {
        public:
            Graphics();
            ~Graphics();

            Renderer*       getRenderer() const { return renderer; }
            MaterialSystem* getMaterialSystem() const { return materialSystem; }
            TextureSystem*  getTextureSystem() const { return textureSystem; }

        private:
            Renderer*       renderer;
            MaterialSystem* materialSystem;
            TextureSystem*  textureSystem;
    };
}
