#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

using std::vector;
using glm::vec2;

namespace Arya
{
    struct Quad
    {
        Quad() 
        {
            textureHandle = 0;
        };

        vec2 relative; // in screen coordinates [-1, 1]
        vec2 pixelOffset; // in pixels
        vec2 pixelSize; // in pixels
        float zValue; // in screen coordinates [-1, 1]

        vec4 fillColor; // in RGBA
        vec4 borderColor; // in RGBA

        GLuint textureHandle;
        vec2 texOffset; // in texcoo [0, 1]
        vec2 texSize; // in texcoo [0, 1]
    };

    class Overlay
    {
        public:
            Overlay();
            ~Overlay();

            void init();

            void addQuad(Quad* quad);
            void removeQuad(Quad* quad);

            void render();

        private:
            vector<Quad*> quads;
   };
}
