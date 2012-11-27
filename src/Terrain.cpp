#include <GL/glew.h>
#include <glm/glm.hpp>

#include "common/Logger.h"
#include "Terrain.h"
#include "Shaders.h"

namespace Arya
{
    Terrain::Terrain(Texture* hm, Texture* ts, Texture* sm) 
    {
        heightMap = hm;
        tileSet = ts;
        splatMap = sm;

        if(!init())
            LOG_WARNING("Could not load terrain shaders");
        generate();
    }

    Terrain::~Terrain()
    {

    }

    bool Terrain::init()
    {
        Shader* terrainVertex = new Shader(VERTEX);
        if(!(terrainVertex->addSourceFile("../shaders/terrain.vert"))) return false;
        if(!(terrainVertex->compile())) return false;

        Shader* terrainFragment = new Shader(FRAGMENT);
        if(!(terrainFragment->addSourceFile("../shaders/terrain.frag"))) return false;
        if(!(terrainFragment->compile())) return false;

        terrainProgram = new ShaderProgram("basic");
        terrainProgram->attach(terrainVertex);
        terrainProgram->attach(terrainFragment);
        if(!(terrainProgram->link())) return false;
        terrainProgram->use();

        return true;
    }

    void Terrain::generate()
    {
        int w, h;
        w = 1025; //heightMap->width;
        h = 1025; //heightMap->height;

        if(!(((w-1) & (w-2)) == 0) || w != h) {
            LOG_WARNING("Heightmap is of the wrong size. Must be of the form 2^n + 1, and square.");
            w = 1025;
            h = 1025;
            LOG_INFO("Heightmap: width and height set to default value (1025).");
        }

        patchCount = 16; // 16 x 16 grid
        int perPatch = (w-1)/patchCount + 1; // default: 1024/16 + 1 = 65x65

        GLfloat* vertexData = new GLfloat(perPatch*perPatch * 2);

        for(int i = 0; i < perPatch; ++i) {
            vertexData[2*i + 0] = 0.0;
            vertexData[2*i + 1] = 0.0;
        }
    }

    void Terrain::generateIndices()
    {
        // make indices for all possibilities
    }
}
