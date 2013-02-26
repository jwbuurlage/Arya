#include "Mesh.h"
#include "common/Logger.h"

namespace Arya
{
    Mesh::Mesh()
    {
        frameCount = 0;
        vaoHandles = 0;
        vertexBuffer = 0;
        vertexCount = 0;
        indexBuffer = 0;
        indexCount = 0;
        primitiveType = 0;
        refCount = 0;
        materialIndex = 0;
    }

    Mesh::~Mesh()
    {
        if(vaoHandles)
        {
            glDeleteVertexArrays(frameCount, vaoHandles);
            delete[] vaoHandles;
        }
        if(indexBuffer)
            glDeleteBuffers(1, &indexBuffer);
        if(vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);
    }

    void Mesh::createVAOs(int count)
    {
        //Delete old handles if they existed
        if(vaoHandles)
        {
            glDeleteVertexArrays(frameCount, vaoHandles);
            delete[] vaoHandles;
        }
        frameCount = count;
        vaoHandles = new GLuint[frameCount];

        glGenVertexArrays(frameCount, vaoHandles);
    }
}
