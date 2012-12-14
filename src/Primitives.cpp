#include "Primitives.h"
#include "common/Logger.h"

namespace Arya
{
    //-----------------------------
    // Triangle
    //-----------------------------

    Triangle::Triangle() : Model()
    {
        Mesh* mesh = createAndAddMesh();

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

        mesh->createVAOs(1);
        glBindVertexArray(mesh->vaoHandles[0]);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    }

    Quad::Quad() : Model()
    {
        GLfloat quadVerts[] = {
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            -1.0f,-1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
        };
        Mesh* mesh = createAndAddMesh();
        mesh->vertexCount = 4;
        mesh->primitiveType = GL_TRIANGLE_STRIP;
        glGenBuffers(1, &mesh->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * 3 * sizeof(GLfloat), quadVerts, GL_STATIC_DRAW);
        mesh->createVAOs(1);
        glBindVertexArray(mesh->vaoHandles[0]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    }
}
