#include "Overlay.h"
#include "Root.h"

namespace Arya
{
    Overlay::Overlay() 
    {
        init();
    }

    Overlay::~Overlay()
    {

    }

    void Overlay::init()
    {
        // make 1 by 1 px vertex buffer
        // inititialize shader
    }

    void Overlay::addQuad(Quad* quad)
    {
        quads.push_back(quad);
    }

    void Overlay::removeQuad(Quad* quad)
    {
        for(vector<Quad*>::iterator it = quads.begin(); it != quads.end();)
        {
            if( *it == quad ) it = quads.erase(it);
            else ++it;
        }
    }

    void render()
    {
        // bind shader
        // render all quads
    }
}
