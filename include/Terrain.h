#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Materials.h"
#include <vector>

using glm::vec2;
using glm::vec3;
using glm::mat4;
using std::vector;

namespace Arya
{
    class Texture;
    class Shader;
    class ShaderProgram;
    class Scene;
    class Camera;
    class File;

    typedef struct
    {
        vec2 position;
        vec2 offset;
        int lod;
    } Patch;

    class Terrain
    {
        public:
            // Tileset needs to have 4 elements
            // heightData is considered to be terrainSize*terrainSize unsigned shorts, with no padding
            Terrain(const char* heightData, int terrainSize, const char* wm, vector<Material*> ts, Texture* cm, Texture* sm);
            ~Terrain(); 
			
            void render(Camera* cam);
            void update(float dt, Scene* curScene);

            bool init();

            GLuint getHeightMapHandle() const { return heightMapHandle; }

            //By default the terrain is [-0.5,0.5]x[0,1]x[-0.5,0.5] multiplied by the scale matrix
            void setScaleMatrix(const mat4& newMat) { scaleMatrix = newMat; }
            const mat4& getScaleMatrix() const { return scaleMatrix; }

        private:
            bool generate();
            bool generateIndices();
            bool generateVAO();

            const char* heightData;
			const char* waterMapName;
            int terrainSize;
            vector<Material*> tileSet;
			Texture* cloudMap;
            Texture* splatMap;
            GLuint heightMapHandle;
			GLuint waterMapHandle;

            mat4 scaleMatrix;

            GLuint vertexBuffer;
            GLuint* indexBuffer;
            GLuint* indexCount;
            GLuint* vaoHandles;

            ShaderProgram* terrainProgram;
			ShaderProgram* waterProgram;

            vector<Patch> patches;
            int patchCount;
            int patchSizeMax;
            int levelMax;
			vec3 lightDirection;

			float time;
    };
}
