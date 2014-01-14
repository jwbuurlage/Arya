/* 
 * Decals are textures on the terrain
 * Decals is a singleton which we may have to change in the future..the render function gets called by terrain (after terrain is rendered)
 * To add Decals you should make and register them 
 */

#pragma once

#include <vector>
using std::vector;

#include <GL/glew.h>
#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;

#include "common/Singleton.h"

namespace Arya
{
	class Texture;
	class ShaderProgram;

	class Decal
	{
		public:
			Decal(Texture* _texture, vec2 _pos, float _scale, vec3 _color);
			~Decal();

			void setPos(vec2 _pos) { pos = _pos; }

			Texture* texture;
			vec2 pos;
			float scale;
			vec3 color;
	};

	class Decals : public Singleton<Decals>
	{
		public:
			Decals();
			~Decals();

			bool init();
			void render();
			void addDecal(Decal* d);
			void removeDecal(Decal* d);
			void clear();

            void enableGrid() { gridEnabled = true; }
            void disableGrid() { gridEnabled = false; }

		private:
			vector<Decal*> decals;

			bool initShaders();
			bool initVertices();

            bool gridEnabled;
            float gridSize;

			int indexCount;

			ShaderProgram* decalProgram;
			GLuint decalVao; // do we want LODs like in terrain?
			GLuint vertexBuffer;
            GLuint indexBuffer;

			GLuint gridVao; // do we want LODs like in terrain?
			GLuint gridVertexBuffer;
            GLuint gridIndexBuffer;
            int gridIndexCount;
	};
}
