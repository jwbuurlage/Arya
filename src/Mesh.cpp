#include "Mesh.h"

namespace Arya
{
    Mesh::Mesh()
    {
        vertexBuffer = 0;
        vertexCount = 0;
        primitiveType = 0;
        refCount = 0;
    }

    Mesh::~Mesh()
    {
        if(vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);
    }
}
