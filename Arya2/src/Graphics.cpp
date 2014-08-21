#include "Graphics.h"
#include "Renderer.h"

namespace Arya
{
    Graphics::Graphics()
    {
        renderer = new Renderer;
        materialSystem = 0;
        textureSystem = 0;
    }

    Graphics::~Graphics()
    {
        delete renderer;
    }

}
