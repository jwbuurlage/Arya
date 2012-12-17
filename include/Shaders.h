// Shader object information
//
// The engine can create a Shader and then attach it to a ShaderProgram
// From that moment on, ShaderProgram will have a pointer to it
// and as soon as the ShaderProgram is deleted it will decrease
// the refCount of its Shaders. When one of them reaches zero the
// Shader is deleted.
// The engine should allocate both Shader and ShaderProgram objects
// The engine should keep a pointer of the ShaderProgram and can forget the Shader pointers
// The engine should delete the ShaderProgram when no longer needed. This will automatically delete the Shaders
#pragma once

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "common/Singleton.h"

using std::string;
using std::vector;
using std::map;

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace Arya
{
    class File;

    enum ShaderType
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    class Shader
    {
        public:
            Shader(ShaderType type);
            ~Shader();

            // Adds a source file to the shader
            bool addSourceFile(string f);

            // Compiles the Shader and sets its handle
            bool compile();

            // Returns the handle
            GLuint getHandle() { return handle; };

        private:
            GLuint handle;
            bool compiled;
            int refCount;

            friend class ShaderProgram;

            vector<File*> sources;
            ShaderType type;
    };

    class ShaderProgram
    {
        public:
            ShaderProgram(string name);
            ~ShaderProgram();

            void attach(Shader* shader);
            bool link();
            void use();

            string getName() { return name; };
            GLuint getHandle() { return handle; };

            GLuint getUniformLocation(const char* name);
            void setUniform1i(const char* name, int val);
            void setUniform1f(const char* name, float val);
            void setUniform2fv(const char* name, vec2 values);
            void setUniform3fv(const char* name, vec3 values);
            void setUniform4fv(const char* name, vec4 values);
            void setUniformMatrix4fv(const char* name, mat4 matrix);

        private:
            bool init();

            GLuint handle;
            bool linked;
            string name;

            vector<Shader*> shaders;
    };
}
