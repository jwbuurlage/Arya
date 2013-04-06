#pragma once

#include <GL/glew.h>

// For input
#include <common/Listeners.h>

namespace Arya
{
	class Scene;
	class Window;
	class ShaderProgram;
	class Texture;

	class MiniMap : public InputListener
	{
		public:
			MiniMap();
			~MiniMap();

			bool init();
			void update(float elapsedTime, Scene* scene);
			void render(Scene* scene);

			GLuint getMMTextureHandle() const;

		private:
			// initialization
			bool initVAOs();
			bool initShader();
			bool initFBO();

			// Texture to hold the minimap
			Texture* mmTexture;
			
			// Camera part
			GLuint cameraCornersVBO;
			GLuint cameraCornersVAO;
			
			// for drawing textures
			GLuint screenVAO;

			// Holds the frame for render-to-texture
			GLuint mmFrameBufferObject;

			// in pixels
			int size;

			ShaderProgram* mmProgram;
			float waitTime;

			// Interface
			Window* mmWindow;
	};
}
