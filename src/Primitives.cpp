#include "Primitives.h"
#include "common/Logger.h"

namespace Arya
{
    //-----------------------------
    // Triangle
    //-----------------------------

    Triangle::Triangle() : Model()
    {
        init();
    }

    void Triangle::init()
    {
        Mesh* mesh = new Mesh;
        addMesh(mesh);

        // Vertices
        GLfloat triangleVertices[] = {
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f
        };

        mesh->vertexCount = 3;
        mesh->primitiveType = GL_TRIANGLES;

        glGenBuffers(1, &mesh->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                mesh->vertexCount * 3 * sizeof(GLfloat),
                triangleVertices,
                GL_STATIC_DRAW);

        glGenVertexArrays(1, &mesh->vaoHandle);

        glBindVertexArray(mesh->vaoHandle);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    }
}
