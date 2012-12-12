#pragma once

#include <GL/glew.h>

namespace Arya
{
    class Mesh
    {
        public:
            //GLuint getVAO() { return vaoHandle; }
            //GLuint getVertexBuffer() { return vertexBuffer; }
            //GLsizei getVertexCount() { return vertexCount; }
            //GLenum getPrimitiveType() { return primitiveType; }

            void addRef(){ refCount++; }
            void release(){ refCount--; }
            int getRefCount() const { return refCount; }

            void createVAOs(int frameCount);

            int frameCount; //1 for static models
            GLuint* vaoHandles; //a list of framecount handles
            GLuint vertexBuffer;
            GLsizei vertexCount; //PER FRAME
            GLuint indexBuffer;
            GLsizei indexCount;
            GLenum primitiveType;
            int materialIndex; //the model has a list of materials

        private:
            int refCount;

            //Only Model and ModelManger can create meshes
            friend class ModelManager;
            friend class Model;
            Mesh();
            ~Mesh();
    };
}
