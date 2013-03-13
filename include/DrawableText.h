#include <GL/glew.h>
#include <string>
using std::string;

namespace Arya
{
	class Font;

	class DrawableText
	{
		public:
			DrawableText();
			~DrawableText();

		private:
			Font* font;
			string text;
			GLuint vao;

			void bake();
	};
}

