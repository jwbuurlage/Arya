#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>
using glm::vec2;

using std::string;

namespace Arya
{
	class Font;

	class DrawableText
	{
		public:
			DrawableText(Font* _font, string _text);
			~DrawableText();

			Font* getFont() const { return font; };
			GLuint getVAO() const { return vao; };

			int getVertexCount() const { return vertexCount; }
			vec2 getSize() const { return size; }

		private:
			Font* font;
			string text;
            GLuint vbo;
			GLuint vao;
			vec2 size;
			int vertexCount;

			void bake();
	};
}

