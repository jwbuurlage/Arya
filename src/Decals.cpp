#include "Decals.h"
#include "Shaders.h"
#include "Root.h"
#include "Materials.h"
#include "Scene.h"
#include "Camera.h"
#include "Textures.h"
#include "Terrain.h"

namespace Arya
{
	////////////////////////////////
	// Decal
	////////////////////////////////

	Decal::Decal(Texture* _texture, vec2 _pos, float _scale, vec3 _color)
	{
		texture = _texture;
		pos = _pos;
		scale = _scale;
		color = _color;
	}

	Decal::~Decal()
	{

	}

	////////////////////////////////
	// Decals
	////////////////////////////////

	template<> Decals* Singleton<Decals>::singleton = 0;

	Decals::Decals()
	{
		decalProgram = 0;
		decalVao = 0;
		indexCount = 0;
        vertexBuffer = 0;
        indexBuffer = 0;

        gridSize = 10.0f;
        gridEnabled = false;
	}

	Decals::~Decals()
	{
        if(decalVao)
            glDeleteVertexArrays(1, &decalVao);
        
        if(indexBuffer)
            glDeleteBuffers(1, &indexBuffer);

        if(vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);

		if(decalProgram)
			delete decalProgram;

	}

	bool Decals::init()
	{
		if(!initShaders())
			return false;
		if(!initVertices())
			return false;

		return true;
	}

	bool Decals::initShaders()
	{
		Shader* decalVertex = new Shader(Arya::VERTEX);
		if(!(decalVertex->addSourceFile("../shaders/terraindecal.vert"))) return false;
		if(!(decalVertex->compile())) return false;

		Shader* decalFragment = new Shader(Arya::FRAGMENT);
		if(!(decalFragment->addSourceFile("../shaders/terraindecal.frag"))) return false;
		if(!(decalFragment->compile())) return false;

		decalProgram = new ShaderProgram("terraindecal");
		decalProgram->attach(decalVertex);
		decalProgram->attach(decalFragment);
		if(!(decalProgram->link())) return false;

		return true;
	}

	void Decals::addDecal(Decal* d)
	{
		decals.push_back(d);
	}

    void Decals::removeDecal(Decal* d)
    {
        for(vector<Decal*>::iterator iter = decals.begin(); iter != decals.end(); )
        {
            if( *iter == d ) iter = decals.erase(iter);
            else ++iter;
        }
    }

	void Decals::clear()
	{
		decals.clear();
	}

	bool Decals::initVertices()
	{
		int lod = 5;
		GLfloat* vertexAndTextureData = new GLfloat[2*lod*lod];

		int index = 0;
		for(int i = 0; i < lod; ++i)
			for(int j = 0; j < lod; ++j)
			{
				vertexAndTextureData[index++] = j * (1.0f/(lod-1));
				vertexAndTextureData[index++] = i * (1.0f/(lod-1));
			}

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 
				sizeof(GLfloat) * index,
                vertexAndTextureData,
                GL_STATIC_DRAW);

		GLuint* indices = new GLuint[2*lod*lod+lod];

		int c = 0;
		int row = 0;
		for(int j = 0; j < lod -1; ++j) {
			if(row++ % 2 == 0)
				for(int i = 0; i < lod; ++i) {
					indices[c++] = j*lod + i;
					indices[c++] = (j+1)*lod + i;
				}
			else
				for(int i = 0; i < lod; ++i) {
					indices[c++] = j*lod + lod - 1 - i;
					indices[c++] = (j+1)*lod + lod - 1 - i;
				}
			indices[c] = indices[c-1];
            ++c;
		}

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(GLuint) * c,
			indices,
			GL_STATIC_DRAW);

		indexCount = c;

		delete[] vertexAndTextureData;
		delete[] indices;

		glGenVertexArrays(1, &decalVao);
		glBindVertexArray(decalVao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glBindVertexArray(0);

        // grid
        // how many points fit on the terrain?
        float tWidth = 1024.0f;
        int n = (int)(2.0 * tWidth / gridSize);

		GLfloat* gridVertexData = new GLfloat[2*n*n];

        int gridIndex = 0;
        for(int i = 0; i < n; ++i)
            for(int j = 0; j < n; ++j)
            {
                gridVertexData[gridIndex++] = -tWidth + j * gridSize;
                gridVertexData[gridIndex++] = -tWidth + i * gridSize;
            }

        
		glGenBuffers(1, &gridVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, gridVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 
				sizeof(GLfloat) * gridIndex,
                gridVertexData,
                GL_STATIC_DRAW);

		GLuint* gridIndices = new GLuint[4*n*n];
        gridIndex = 0;
        // horizontal lines
		for(int i = 0; i < n; ++i)
            for(int j = 0; j < n-1; ++j) {
                gridIndices[gridIndex++] = i * n + j;
                gridIndices[gridIndex++] = i * n + j + 1;
                gridIndices[gridIndex++] = j * n + i;
                gridIndices[gridIndex++] = (j+1) * n + i;

            }

		glGenBuffers(1, &gridIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(GLuint) * gridIndex,
			gridIndices,
			GL_STATIC_DRAW);

		indexCount = c;
        gridIndexCount = gridIndex;

        delete[] gridIndices;
		delete[] gridVertexData;

		glGenVertexArrays(1, &gridVao);
		glBindVertexArray(gridVao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, gridVertexBuffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBuffer);
        glBindVertexArray(0); 

		return true;
	}

	void Decals::render()
	{
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		decalProgram->use();
		glBindVertexArray(decalVao);

		decalProgram->setUniformMatrix4fv("vpMatrix", Root::shared().getScene()->getCamera()->getVPMatrix());
		decalProgram->setUniform1i("heightMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Root::shared().getScene()->getTerrain()->getHeightMapHandle());

		decalProgram->setUniform2fv("oneOverTerrainSize", vec2(1.0/2048.0, 1.0/2048.0));

		glBindVertexArray(decalVao);

		for(int i = 0; i < decals.size(); ++i)
		{
			decalProgram->setUniform1i("decalTexture", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, decals[i]->texture->handle);
			decalProgram->setUniform3fv("decalColor", decals[i]->color);
			decalProgram->setUniform1f("scaleFactor", decals[i]->scale);
			decalProgram->setUniform2fv("offset", decals[i]->pos);

            glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, (void*)0);
		}

        // if grid enabled draw it...
        if(gridEnabled)
        {
			decalProgram->setUniform1i("decalTexture", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, TextureManager::shared().getTexture("white")->handle);
			decalProgram->setUniform3fv("decalColor", vec3(0.5f));
			decalProgram->setUniform1f("scaleFactor", 1.0f);
			decalProgram->setUniform2fv("offset", vec2(0.0f));

            glBindVertexArray(gridVao);
            glDrawElements(GL_LINES, gridIndexCount, GL_UNSIGNED_INT, (void*)0);
        }

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}
