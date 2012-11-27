#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

using glm::vec2;
using glm::vec3;
using std::vector;

namespace Arya
{
    class Texture;
    class ShaderProgram;

    typedef struct
    {
        vec3 position;
        vec2 offset;
        int lod;
    } Patch;

    class Terrain
    {
        public:
            Terrain(Texture* hm, Texture* ts, Texture* sm);
            ~Terrain(); 

            void render();

        private:
            bool init();
            void generate();
            void generateIndices();

            Texture* heightMap;
            Texture* tileSet;
            Texture* splatMap;

            GLuint vertexBuffer;
            GLuint* indexBuffer;
            GLuint* indexCount;

            ShaderProgram* terrainProgram;

            vector<Patch> patches;
            int patchCount;
            int patchSizeMax;
            int levelMax;
    };
}
