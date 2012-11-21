#include <string>
#include <vector>

using std::string;
using std::vector;

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
            Shader(ShaderType _type) { type = _type; };
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
            ShaderProgram() { };
            ~ShaderProgram() { };

            bool init();
            void attach(Shader* shader);
            bool link();
            bool use();

        private:
            GLuint handle;
            bool linked;
    };

    class ShaderManager
    {
        public:
            ShaderManager() { };
            ~ShaderManager() { };

            bool init();
    };
}
