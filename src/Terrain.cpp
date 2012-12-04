#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/log_base.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/Logger.h"
#include "Terrain.h"
#include "Shaders.h"
#include "Scene.h"
#include "Camera.h"
#include "Textures.h"

using glm::log;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::distance;

namespace Arya
{
    Terrain::Terrain(Texture* hm, vector<Texture*> ts, Texture* sm) 
    {
        heightMap = hm;
        tileSet = ts;
        if(!(tileSet.size() == 4))
            LOG_WARNING("Tileset is of wrong size");
        splatMap = sm;
        vertexBuffer = 0;
        indexBuffer = 0;
        indexCount = 0;
        terrainProgram = 0;
        patchCount = 0;
        patchSizeMax = 0;
        levelMax = 0;
    }

    Terrain::~Terrain()
    {
        if(indexBuffer)
            delete[] indexBuffer;
        if(indexCount)
            delete[] indexCount;
        if(terrainProgram)
            delete terrainProgram;
    }

    bool Terrain::init()
    {
        if(heightMap == 0 || splatMap == 0) return false;
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

        patchCount = 16; // default: 16 x 16 grid
        patchSizeMax = (w-1)/patchCount + 1; // default: 1024/16 + 1 = 65x65

        GLfloat* vertexData = new GLfloat[patchSizeMax*patchSizeMax * 2];

        scaleMatrix = glm::scale(mat4(1.0), vec3((float)w));
        float perVertex = (1.0f / ((patchSizeMax - 1) * patchCount));

        for(int i = 0; i < patchSizeMax; ++i)
            for(int j = 0; j < patchSizeMax; ++j) {
                vertexData[2*i*patchSizeMax + 2*j + 0] = j*perVertex;
                vertexData[2*i*patchSizeMax + 2*j + 1] = i*perVertex;
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
                p.offset = vec2((1.0 / patchCount)*j, (1.0 / patchCount)*i);
                p.position = (vec2(-0.5) + p.offset);
                p.position.x *= w;
                p.position.y *= w;
                p.lod = -1;
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

        levelMax = log((float)(patchSizeMax-1), 2.0f) + 1;

        indexBuffer = new GLuint[levelMax];
        indexCount = new GLuint[levelMax];

        glGenBuffers(levelMax, indexBuffer);

        GLuint* indices = new GLuint[patchSizeMax * (patchSizeMax-1) * 2];

        for(int l = 0; l < levelMax; ++l)
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

        if(!generateVAO()) return false;
        return true;
    }

    bool Terrain::generateVAO()
    {
        vaoHandles = new GLuint[levelMax];
        glGenVertexArrays(levelMax, vaoHandles);

        for(int l = 0; l < levelMax; ++l) 
        {
            glBindVertexArray(vaoHandles[l]);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[l]);
        }

        // prevent other code from mesing up our vao
        glBindVertexArray(0);

        return true;
    }

    void Terrain::update(float dt, Scene* curScene)
    {
        // update patches LOD
        vec3 camPos = curScene->getCamera()->getRealCameraPosition();
        for(int i = 0; i < patches.size(); ++i) {
            Patch& p = patches[i];
            vec3 pPos = vec3(p.position, -100.0f);
            if(distance(pPos, camPos) < 300.0f)
                p.lod = 0;
            else if(distance(pPos, camPos) < 600.0f)
                p.lod = 3;
            else
                p.lod = 5;
        }
    }

    //---------------------------------------
    // RENDER
    //---------------------------------------

    void Terrain::render(Camera* cam)
    {
        terrainProgram->use();
        terrainProgram->setUniformMatrix4fv("vpMatrix", cam->getVPMatrix());
        terrainProgram->setUniformMatrix4fv("scaleMatrix", scaleMatrix);

        // heightmap
        terrainProgram->setUniform1i("heightMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMap->handle);

        // splatmap
        terrainProgram->setUniform1i("splatTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, splatMap->handle);

        // texture 1 to 4
        terrainProgram->setUniform1i("texture1", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tileSet[0]->handle);

        terrainProgram->setUniform1i("texture2", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, tileSet[1]->handle);

        terrainProgram->setUniform1i("texture3", 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, tileSet[2]->handle);

        terrainProgram->setUniform1i("texture4", 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, tileSet[3]->handle);

        for(int i = 0; i < patches.size(); ++i) {
            Patch& p = patches[i];
            if(p.lod < 0) continue;
            terrainProgram->setUniform2fv("patchOffset", p.offset);
            terrainProgram->setUniform2fv("patchPosition", p.position);

            glBindVertexArray(vaoHandles[p.lod]);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount[p.lod], GL_UNSIGNED_INT, (void*)0);
        }
    }
}
