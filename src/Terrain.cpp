#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/log_base.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Fonts.h"
#include "common/Logger.h"
#include "Terrain.h"
#include "Shaders.h"
#include "Scene.h"
#include "Camera.h"
#include "Textures.h"
#include "Files.h"

#include <string>
using std::string;

using glm::log;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::distance;

namespace Arya
{
    Terrain::Terrain(const char* _heightData, int _terrainSize, const char* wm, vector<Material*> ts, Texture* cm, Texture* sm) 
    {
        heightData = _heightData;
        terrainSize = _terrainSize;
		waterMapName = wm;
        tileSet = ts;
        if(!(tileSet.size() == 4))
            LOG_WARNING("Tileset is of wrong size");
		cloudMap = cm;
        splatMap = sm;
        vertexBuffer = 0;
        indexBuffer = 0;
        indexCount = 0;
        terrainProgram = 0;
        patchCount = 0;
        patchSizeMax = 0;
        levelMax = 0;

        heightMapHandle = 0;
		waterMapHandle = 0;
        //hFile = 0;
		this->lightDirection=lightDirection;

		time=0;
    }

    Terrain::~Terrain()
    {
        if(indexBuffer) {
            glDeleteBuffers(levelMax, indexBuffer);
            delete[] indexBuffer;
        }
        if(indexCount)
            delete[] indexCount;
        if(terrainProgram) 
            delete terrainProgram;

        if(vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);

        if(heightMapHandle)
            glDeleteTextures(1, &heightMapHandle);

		if(waterMapHandle)
			glDeleteTextures(1, &waterMapHandle);
    }

    bool Terrain::init()
    {
        if(heightData == 0 || waterMapName == 0 || cloudMap == 0 || splatMap == 0) return false;

        for(unsigned int i = 0; i < tileSet.size(); ++i) {
            if(!tileSet[i]) return false;
            glBindTexture(GL_TEXTURE_2D, tileSet[i]->texture->handle);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

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

		Shader* waterVertex = new Shader(VERTEX);
        if(!(waterVertex->addSourceFile("../shaders/water.vert"))) return false;
        if(!(waterVertex->compile())) return false;

        Shader* waterFragment = new Shader(FRAGMENT);
        if(!(waterFragment->addSourceFile("../shaders/water.frag"))) return false;
        if(!(waterFragment->compile())) return false;

        waterProgram = new ShaderProgram("basic");
        waterProgram->attach(waterVertex);
        waterProgram->attach(waterFragment);
        if(!(waterProgram->link())) return false;

        //This happens in the game engine now
        //string heightMapString(heightMapName);
        //heightMapString.insert(0, "textures/");
        //hFile = FileSystem::shared().getFile(heightMapString);
        //if(!hFile) return false;

        glGenTextures(1, &heightMapHandle);
        glBindTexture(GL_TEXTURE_2D, heightMapHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, terrainSize, terrainSize, 0, GL_RED, GL_UNSIGNED_SHORT, heightData);

		// load in watermap
        string waterMapString(waterMapName);
        waterMapString.insert(0, "textures/");
        File* wFile = FileSystem::shared().getFile(waterMapString);
        if(!wFile) return false;

        glGenTextures(1, &waterMapHandle);
        glBindTexture(GL_TEXTURE_2D, waterMapHandle);
		
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, terrainSize, terrainSize, 0, GL_RED, GL_UNSIGNED_SHORT, wFile->getData());
		


		glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, cloudMap->handle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		

		FileSystem::shared().releaseFile(wFile);

        return true;
    }

    bool Terrain::generate()
    {
        int w, h;
        w = terrainSize;
        h = terrainSize;

        if(!(((w-1) & (w-2)) == 0) || w != h) {
            LOG_WARNING("Heightmap is of the wrong size. Must be of the form 2^n + 1, and square.");
            w = 1025;
            h = 1025;
            LOG_INFO("Heightmap: width and height set to default value (1025).");
        }

        patchCount = 16; // default: 16 x 16 grid
        patchSizeMax = (w-1)/patchCount + 1; // default: 1024/16 + 1 = 65x65

        GLfloat* vertexData = new GLfloat[patchSizeMax*patchSizeMax * 2];

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
                p.position = (vec2(-0.5 + 0.5 / patchCount) + p.offset);
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

        GLuint* indices = new GLuint[patchSizeMax * (patchSizeMax-1) * 2 + (patchSizeMax - 1)];

        for(int l = 0; l < levelMax; ++l)
        {
            int c = 0;
            //int levelSize = (patchSizeMax-1)/(1 << l) + 1;
            // level l
            int row = 0;
            for(int j = 0; j < patchSizeMax - 1; j += 1 << l) {
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
                indices[c] = indices[c-1];
                c++;
            }

            indexCount[l] = c;
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
		time+=dt;

        // update patches LOD
        vec3 camPos = curScene->getCamera()->getRealCameraPosition();

        for(unsigned int i = 0; i < patches.size(); ++i) {
            Patch& p = patches[i];
            vec3 pPos = vec3(p.position.x, -100.0f, p.position.y);
            if(distance(pPos, camPos) < 300.0f)
                p.lod = 0;
            else if(distance(pPos, camPos) < 600.0f)
                p.lod = 1;
            else if(distance(pPos, camPos) < 900.0f)
                p.lod = 2;
            else if(distance(pPos, camPos) < 1200.0f)
                p.lod = 3;
            else if(distance(pPos, camPos) < 1500.0f)
                p.lod = 4;
            else
                p.lod = levelMax -1;
        }
    }

    //---------------------------------------
    // RENDER
    //---------------------------------------

    void Terrain::render(Camera* cam)
    {
        static mat4 biasMatrix(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f
        );

        terrainProgram->use();
        terrainProgram->setUniformMatrix4fv("vpMatrix", cam->getVPMatrix());
		terrainProgram->setUniformMatrix4fv("viewMatrix", cam->getVMatrix());
        terrainProgram->setUniformMatrix4fv("scaleMatrix", scaleMatrix);
        terrainProgram->setUniformMatrix4fv("lightOrthoMatrix", biasMatrix * Root::shared().getScene()->getLightOrthoMatrix());
		terrainProgram->setUniform3fv("lightDirection", vec3(0.7,0.7,0.0));

        // heightmap
        terrainProgram->setUniform1i("heightMap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMapHandle);

        // splatmap
        terrainProgram->setUniform1i("splatTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, splatMap->handle);

        // texture 1 to 4
		terrainProgram->setUniform4fv("parameters1",tileSet[0]->getParameters());
		terrainProgram->setUniform4fv("parameters2",tileSet[1]->getParameters());
		terrainProgram->setUniform4fv("parameters3",tileSet[2]->getParameters());
		terrainProgram->setUniform4fv("parameters4",tileSet[3]->getParameters());

        terrainProgram->setUniform1i("texture1", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,tileSet[0]->texture->handle);

        terrainProgram->setUniform1i("texture2", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, tileSet[1]->texture->handle);

        terrainProgram->setUniform1i("texture3", 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, tileSet[2]->texture->handle);

        terrainProgram->setUniform1i("texture4", 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, tileSet[3]->texture->handle);

        terrainProgram->setUniform1i("shadowMap", 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, Root::shared().getScene()->getShadowDepthTextureHandle());

        for(unsigned int i = 0; i < patches.size(); ++i) {
            Patch& p = patches[i];
            if(p.lod < 0) continue;
            terrainProgram->setUniform2fv("patchOffset", p.offset);
            terrainProgram->setUniform2fv("patchPosition", p.position);

            glBindVertexArray(vaoHandles[p.lod]);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount[p.lod], GL_UNSIGNED_INT, (void*)0);
        }


		glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);

		waterProgram->use();
        waterProgram->setUniformMatrix4fv("vpMatrix", cam->getVPMatrix());
		waterProgram->setUniformMatrix4fv("viewMatrix", cam->getVMatrix());
        waterProgram->setUniformMatrix4fv("scaleMatrix", scaleMatrix);
		waterProgram->setUniform3fv("lightSource",vec3(0.7,0.7,0.0));

		// watermap
        waterProgram->setUniform1i("waterMap", 6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, waterMapHandle);

		// heightmap
        waterProgram->setUniform1i("heightMap", 0);

		//cloudmap
		waterProgram->setUniform1i("clouds", 7);
		glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, cloudMap->handle);

		for(unsigned int i = 0; i < patches.size(); ++i) {
            Patch& p = patches[i];
            if(p.lod < 0) continue;
            waterProgram->setUniform2fv("patchOffset", p.offset);
            waterProgram->setUniform2fv("patchPosition", p.position);
			waterProgram->setUniform1f("time", time);

            glBindVertexArray(vaoHandles[p.lod]);
            glDrawElements(GL_TRIANGLE_STRIP, indexCount[p.lod], GL_UNSIGNED_INT, (void*)0);
        }

		glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }
}
