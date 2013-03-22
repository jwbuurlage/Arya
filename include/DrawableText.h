#include <GL/glew.h>
#include <string>
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

		private:
			Font* font;
			string text;
			GLuint vao;
			int vertexCount;

			void bake();
	};
}

