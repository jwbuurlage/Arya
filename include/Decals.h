/* To add Decals you shoulde make AND register them 
 *
 *
 */
#pragma once

#include <vector>
using std::vector;

#include <GL/glew.h>
#include <glm/glm.hpp>
using glm::vec2;

namespace Arya
{
	class Texture;
	class ShaderProgram;

	class Decal
	{
		public:
			Decal(Texture* _texture, vec2 _pos, vec2 _size);
			~Decal();

			Texture* texture;
			vec2 pos;
			vec2 size;
	};

	class Decals
	{
		public:
			Decals();
			~Decals();

			void render();

		private:
			vector<Decal> decals;

			ShaderProgram* decalProgram;
			GLuint decalVao; // do we want LODs like in terrain?
	};
}
