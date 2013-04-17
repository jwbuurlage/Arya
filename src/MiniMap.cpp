#include "MiniMap.h"
#include "Interface.h"
#include "Textures.h"
#include "Root.h"
#include "Shaders.h"
#include "Scene.h"
#include "Terrain.h"
#include "FogMap.h"
#include "Camera.h"

#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::mat4;

#define UPDATE_TIME 0.1f

namespace Arya
{
	MiniMap::MiniMap()
	{
		cameraCornersVBO = 0;
		cameraCornersVAO = 0;
		screenVAO = 0;
		mmFrameBufferObject = 0;
		size = 0; 
        vertexBuffer = 0;

		mmWindow = 0;
		mmProgram = 0;
		mmTexture = 0;

		waitTime = 0.0f;
	}

	MiniMap::~MiniMap()
	{
		if(mmWindow) delete mmWindow;
		if(mmProgram) delete mmProgram;
		if(mmCameraLinesProgram) delete mmCameraLinesProgram;

        if(vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);
        if(cameraCornersVBO)
            glDeleteBuffers(1, &cameraCornersVBO);
        if(screenVAO)
            glDeleteVertexArrays(1, &screenVAO);
        if(cameraCornersVAO)
            glDeleteVertexArrays(1, &cameraCornersVAO);
	}

	GLuint MiniMap::getMMTextureHandle() const
	{
		return mmTexture->handle; 
	}

	bool MiniMap::init()
	{
		size = 200;

		GLuint tmpHandle;
		glGenTextures(1, &tmpHandle);
		mmTexture = TextureManager::shared().createTextureFromHandle("minimap", tmpHandle);

		if(!initShader()) {
			LOG_ERROR("Could not initialize mini-map shaders");
			return false;
		}
		if(!initFBO()) {
			LOG_ERROR("Could not initialize mini-map fbo");
		   	return false;
		}
		if(!initVAOs()) {
			LOG_ERROR("Could not initialize mini-map vao");
			return false;
		}

		mmWindow = new Window(
				vec2(-1.0, -1.0), vec2(0.0, 0.0), vec2(size),
				mmTexture, WINDOW_DRAGGABLE, "Minimap");
		Interface::shared().makeActive(mmWindow);

		return true;
	}

	bool MiniMap::initShader()
	{
		mmProgram = new ShaderProgram("minimap",
				"../shaders/minimap.vert",
				"../shaders/minimap.frag");
		if(!mmProgram->isValid()) return false;

		mmCameraLinesProgram = new ShaderProgram("cameraLines",
				"../shaders/whitelines.vert",
				"../shaders/whitelines.frag");
		if(!mmCameraLinesProgram->isValid()) return false;

		return true;
	}

	bool MiniMap::initFBO()
	{
		Root::shared().checkForErrors("before minimap fbo");

		glGenFramebuffers(1, &mmFrameBufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, mmFrameBufferObject);

        glBindTexture(GL_TEXTURE_2D, mmTexture->handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mmTexture->handle, 0);

		GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, drawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Root::shared().checkForErrors("after minimap fbo");

		return true;
	}

	bool MiniMap::initVAOs()
	{
		/* FOR TEXTURES */
		GLfloat vertexAndTextureData[] = {
			-1.0f, 	-1.0f, 	0.0f, 	1.0f,
			1.0f, 	-1.0f, 	1.0f, 	1.0f,
			-1.0f, 	 1.0f, 	0.0f, 	0.0f,
			1.0f, 	 1.0f, 	1.0f, 	0.0f,
		};

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER,
				sizeof(vertexAndTextureData),
				vertexAndTextureData,
				GL_STATIC_DRAW);

		glGenVertexArrays(1, &screenVAO);
		glBindVertexArray(screenVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 4*sizeof(GLfloat), reinterpret_cast<GLbyte*>(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 4*sizeof(GLfloat), reinterpret_cast<GLbyte*>(8));

		/* FOR CAMERA */
		GLfloat cameraVertices[] = {
			-0.5f, -0.5f,
			-0.5f,  0.5f,
			 0.5f, 	0.5f,
			 0.5f, -0.5f
		};

		glGenBuffers(1, &cameraCornersVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cameraCornersVBO);
		glBufferData(GL_ARRAY_BUFFER,
				sizeof(cameraVertices),
				cameraVertices,
				GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &cameraCornersVAO);
		glBindVertexArray(cameraCornersVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);

		glBindVertexArray(0);

		return true;
	}

	void MiniMap::update(float elapsedTime, Scene* scene)
	{
		waitTime += elapsedTime;
		if(waitTime < UPDATE_TIME)
			return;
		waitTime -= UPDATE_TIME;

		vec3 camPos = scene->getCamera()->getPosition();
		mat4 terrainScale = scene->getTerrain()->getScaleMatrix();
		double xSize = terrainScale[0][0];
		double zSize = terrainScale[2][2];
		vec2 relativePos = vec2(camPos.x / xSize, - (camPos.z / zSize));

		/* FOR CAMERA */
		GLfloat cameraVertices[] = {
			relativePos.x - 0.1f, relativePos.y - 0.1f,
			relativePos.x - 0.1f, relativePos.y + 0.1f,
			relativePos.x + 0.1f, relativePos.y + 0.1f,
			relativePos.x + 0.1f, relativePos.y - 0.1f
		};

		glBindBuffer(GL_ARRAY_BUFFER, cameraCornersVBO);
		glBufferData(GL_ARRAY_BUFFER,
				sizeof(cameraVertices),
				cameraVertices,
				GL_DYNAMIC_DRAW);

		render(scene);
	}

	void MiniMap::render(Scene* scene)
	{
        glBindFramebuffer(GL_FRAMEBUFFER, mmFrameBufferObject);
        glViewport(0, 0, size, size);

		mmProgram->use();

		// TEXTURES
        mmProgram->setUniform1i("heightmap", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,scene->getTerrain()->getHeightMapHandle());

        mmProgram->setUniform1i("fogmap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, scene->getFogMap()->getFogMapTextureHandle());

		glBindVertexArray(screenVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		mmCameraLinesProgram->use();
		glBindVertexArray(cameraCornersVAO);
		glDrawArrays(GL_LINE_LOOP, 0, 4);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Root::shared().getWindowWidth(), Root::shared().getWindowHeight());

	}
}
