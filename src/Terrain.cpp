#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/log_base.hpp>

#include "common/Logger.h"
#include "Terrain.h"
#include "Shaders.h"

using glm::log;

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
        patchSizeMax = (w-1)/patchCount + 1; // default: 1024/16 + 1 = 65x65

        GLfloat* vertexData = new GLfloat(patchSizeMax*patchSizeMax * 2);

        float perVertex = 1.0f / (patchSizeMax - 1);

        for(int i = 0; i < patchSizeMax; ++i) 
            for(int j = 0; j < patchSizeMax; ++j) {
                vertexData[2*i + 0] = i*perVertex;
                vertexData[2*i + 1] = j*perVertex;
            }

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 
                sizeof(GLfloat) * patchSizeMax*patchSizeMax * 2, 
                vertexData,
                GL_STATIC_DRAW);

        delete[] vertexData;

        // patches
        for(int i = 0; i < patchCount; ++i)
            for(int j = 0; j < patchCount; ++j) {
                Patch p;
                p.position = vec2(0.0, 0.0);
                p.offset = vec2(0.0, 0.0);
                p.lod = 0;
                patches.push_back(p);
            }

        generateIndices();
    }

    void Terrain::generateIndices()
    {
        // make indices for all possibilities
        int levels = log(patchSizeMax-1, 2);
    }
}
