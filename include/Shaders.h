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

namespace Arya
{
    struct File;

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
            void setUniform2fv(const char* name, vec2 values);
            void setUniformMatrix4fv(const char* name, mat4 matrix);

        private:
            bool init();

            GLuint handle;
            bool linked;
            string name;
    };
}
