#include "Mesh.h"

namespace Arya
{
    Mesh::Mesh()
    {
        vertexBuffer = 0;
        colorBuffer = 0;
        vertexCount = 0;
        primitiveType = 0;

        hasColor = false;
    }
}
