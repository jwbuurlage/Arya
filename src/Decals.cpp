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
	}

	Decals::~Decals()
	{
		if(decalProgram)
			delete decalProgram;

		if(decalVao)
			glDeleteVertexArrays(1, &decalVao);
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
		for(int i = 0; i < decals.size(); ++i)
			if(decals[i] == d)
			{
				  decals.erase(decals.begin() + i);
				  return;
			  }
			
		LOG_WARNING("Trying to delete decal which does not exist");
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
			indices[c++] = indices[c-1];
		}

		GLuint indexBuffer;
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

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}
