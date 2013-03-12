#include "DrawableText.h"
#include "Fonts.h"
#include <vector>
using std::vector;

namespace Arya
{
	DrawableText::DrawableText(Font* font, string text)
	{
		vao = 0;

		font = _font;
		text = _text;

		bake();
	}

	~DrawableText::DrawableText()
	{

	}

	DrawableText::bake()
	{
		if(vao)
		{
			LOG_ERROR("Already baked DrawableText with string: " << text);
			return;
		}

		vector<GLfloat> vertexAndTextureData;
		vertexAndTextureData.reserve(text.length() * 16);

		float xpos = 0.0f, ypos = 0.0f;
		stbtt_aligned_quad q;

		int index = 0;
		float currentOffset = 0.0f;
		for(int i = 0; i < text.length(); ++i)
		{
			// b---d
			// | A |
			// a---c
			//
			// we set a_1 = (0, 0) and the rest is in pixel space

			// we are at char text[i]
			stbtt_GetBakedQuad(font->baked, 512, 512, text[i], &xpos, &ypos, &q, true);

			// a
			vertexAndTextureData[index++] = currentOffset;  				// x
			vertexAndTextureData[index++] = -q.y1;		   	 				// y
			vertexAndTextureData[index++] = q.s0; 		    				// s
			vertexAndTextureData[index++] = 1 - q.t0; 						// t

			// b
			vertexAndTextureData[index++] = currentOffset;  				// x
			vertexAndTextureData[index++] = -q.y1 + (q.y1 - q.y0);			// y
			vertexAndTextureData[index++] = q.s0; 		    				// s
			vertexAndTextureData[index++] = 1 - q.t0 + (q.t1 - q.t0); 		// t

			// c
			vertexAndTextureData[index++] = currentOffset + (q.x1 - q.x0);	// x
			vertexAndTextureData[index++] = -q.y1;							// y
			vertexAndTextureData[index++] = q.s0 + (q.s1 - q.s0); 			// s
			vertexAndTextureData[index++] = 1 - q.t0; 						// t

			// d
			vertexAndTextureData[index++] = currentOffset + (q.x1 - q.x0);	// x
			vertexAndTextureData[index++] = -q.y1 + (q.y1 - q.y0);			// y
			vertexAndTextureData[index++] = q.s0 + (q.s1 - q.s0); 			// s
			vertexAndTextureData[index++] = 1 - q.t0 + (q.t1 - q.t0); 		// t
		}

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 
				sizeof(GLfloat) * vertexAndTextureData.size(),
                &vertexAndTextureData[0],
                GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(GLfloat) * 2, (void*)0);
		glBindVertexArray(0);

		// we should be able to draw now using this vao
	}
}

