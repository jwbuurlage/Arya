#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/log_base.hpp>

#include "common/Logger.h"
#include "Terrain.h"
#include "Shaders.h"
#include "Scene.h"
#include "Camera.h"
#include "Textures.h"

using glm::log;
using glm::vec3;
using glm::distance;

namespace Arya
{
    Terrain::Terrain(Texture* hm, Texture* ts, Texture* sm) 
    {
        heightMap = hm;
        tileSet = ts;
        splatMap = sm;
    }

    Terrain::~Terrain()
    {
        delete[] indexBuffer;
        delete[] indexCount;
        delete terrainProgram;
    }

    bool Terrain::init()
    {
        if(heightMap == 0) return false;
        if(!generate()) return false;

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

        return true;
    }

    bool Terrain::generate()
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

        GLfloat* vertexData = new GLfloat[patchSizeMax*patchSizeMax * 2];

        float perVertex = 1.0f / (patchSizeMax - 1);

        for(int i = 0; i < patchSizeMax; ++i) 
            for(int j = 0; j < patchSizeMax; ++j) {
                vertexData[2*i*patchSizeMax + 2*j + 0] = i*perVertex;
                vertexData[2*i*patchSizeMax + 2*j + 1] = j*perVertex;
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
                p.offset = vec2(j*(patchCount - 1), i*(patchCount - 1));
                p.position = vec2(-w/2 + (w-1)/patchCount*j, -h/2+ (h-1)/patchCount*j);
                p.lod = 0;
                patches.push_back(p);
            }

        generateIndices();

        return true;
    }

    bool Terrain::generateIndices()
    {
        // make indices for all possibilities
        // level 0: patchSizeMax^2
        // ...
        // level levels: 1^2

        int levels = log((float)(patchSizeMax-1), 2.0f) + 1;
        LOG_INFO("levels: " << levels);

        indexBuffer = new GLuint[levels];
        indexCount = new GLuint[levels];

        glGenBuffers(levels, indexBuffer);

        GLuint* indices = new GLuint[patchSizeMax * (patchSizeMax-1) * 2];

        for(int l = 0; l < levels; ++l)
        {
            int c = 0;
            int levelSize = (patchSizeMax-1)/(1 << l) + 1;
            indexCount[l] = levelSize * (levelSize-1) * 2;
            // level l
            int row = 0;
            for(int j = 0; j < patchSizeMax - 1; j += 1 << l)
                if(row++ % 2 == 0)
                    for(int i = 0; i < patchSizeMax; i += 1 << l) {
                        indices[c++] = j*patchSizeMax + i;
                        indices[c++] = (j+(1 << l))*patchSizeMax + i;
                    }
                else
                    for(int i = 0; i < patchSizeMax; i += 1 << l) {
                        indices[c++] = j*patchSizeMax + patchSizeMax - 1 - i;
                        indices[c++] = (j+(1 << l))*patchSizeMax + patchSizeMax - 1 - i;
                   }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[l]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(GLuint) * indexCount[l],
                    indices,
                    GL_STATIC_DRAW);
        }

        delete[] indices;

        return true;
    }

    void Terrain::update(float dt, Scene* curScene)
    {
        // update patches LOD
        vec3 camPos = curScene->getCamera()->getRealCameraPosition();
        for(int i = 0; i < patches.size(); ++i) {
            Patch p = patches[i];
            vec3 pPos = vec3(p.position, 0.0);
            if(distance(pPos, camPos) < 20.0f)
                p.lod = 0;
            else if(distance(pPos, camPos) < 40.0f)
                p.lod = 1;
            else
                p.lod = 2;
        }
    }

    //---------------------------------------
    // RENDER
    //---------------------------------------

    void Terrain::render()
    {
        terrainProgram->use();
        terrainProgram->setUniform1i("heightMap", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMap->handle);

        glBindVertexArray(vertexBuffer);
        for(int i = 0; i < patches.size(); ++i) {
            Patch p = patches[i];
            terrainProgram->setUniform2fv("patchOffset", p.offset);
            terrainProgram->setUniform2fv("patchPosition", p.position);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[3]);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount[3], GL_UNSIGNED_INT, (void*)0);
        }
    }
}
