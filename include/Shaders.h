#pragma once

#include <string>
#include <vector>
#include <map>

#include "common/Singleton.h"

using std::string;
using std::vector;
using std::map;

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
            Shader(ShaderType type) { this->type = type; };
            ~Shader() { };

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

        private:
            bool init();

            GLuint handle;
            bool linked;
            string name;
    };
}
