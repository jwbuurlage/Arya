#include "Overlay.h"
#include "Root.h"
#include <iostream>
#include "Interface.h"
#include "Fonts.h"
#include "Textures.h"
#include "Overlay.h"
#include "Shaders.h"
#include "common/Logger.h"
#include <sstream>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include "../ext/stb_truetype.h"
using std::vector;

#define OFFSET_X 30.0f
#define OFFSET_Y 30.0f

namespace Arya
{
    template<> Interface* Singleton<Interface>::singleton = 0;

	////////////////////////////////
	// InterfaceElement
	///////////////////////////////

	InterfaceElement::InterfaceElement(vec2 _position, vec2 _size)
	{
		position = _position;
		size = _size;
	}

	////////////////////////////////
	// Window
	///////////////////////////////

	Window::Window(vec2 _position, vec2 _size, vec4 _backgroundColor)
		: InterfaceElement(_size, _position)
	{
		backgroundColor = _backgroundColor;
		isActive = false;
	}

	void Window::addChild(InterfaceElement* ele)
	{
		childElements.push_back(ele);
	}

	void Window::removeChild(InterfaceElement* ele)
	{
		for(int i = 0; i < childElements.size(); ++i)
		{
			if(childElements[i] == ele)
				childElements.erase(childElements.begin() + i);
		}
	}

	void Window::setActiveState(bool active)
	{
		isActive = active;
	}

	void Window::draw()
	{
		if(!isActive) return;

		ShaderProgram* p = Interface::shared().getTexturedRectProgram();
		GLuint v = Interface::shared().getOnePxRectVAO();
		// draw background
		// bind shader overlay
		p->use();
		glBindVertexArray(v);

		p->setUniform1i("texture1", 0);
		glActiveTexture(GL_TEXTURE0);
		p->setUniform4fv("uColor", backgroundColor);
		glBindTexture(GL_TEXTURE_2D, TextureManager::shared().getTexture("white")->handle);

		p->setUniform2fv("screenSize", size);
		p->setUniform2fv("screenPosition", position);
		p->setUniform2fv("texOffset", vec2(0.0f));
		p->setUniform2fv("texSize", vec2(1.0f));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// draw childs
		for(int i = 0; i << childElements.size(); ++i)
			childElements[i]->draw();
	}

	////////////////////////////////
	// Label
	///////////////////////////////

	Label::Label(vec2 _position, vec2 _size)
		: InterfaceElement(_size, _position)
	{

	}

	////////////////////////////////
	// Interface
	///////////////////////////////

	Interface::Interface()
	{
		offsetFPS = 0.0;
		time = 0.0;
		count = 0;
		overlay = 0; 
	}

	bool Interface::init()
	{
		// inititialize shader
		if(!initShaders()) return false;

		LOG_INFO("Initialized overlay shaders");

		// make 1 by 1 px vertex buffer
		// inititialize shader
		GLfloat vertices[] = {
			0.0,        0.0,
			1.0,        0.0,
			0.0,        1.0,
			1.0,        1.0
		};

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); 

		glGenVertexArrays(1, &onePxRectVAO);
		glBindVertexArray(onePxRectVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindVertexArray(0);

		// OVERLAY
		if(!overlay) overlay = new Overlay(texturedRectProgram, onePxRectVAO);
		if(!overlay->init())
		{
			LOG_ERROR("Could not initialize overlay");
			return false;
		}

		// fonts
		Font* font = FontManager::shared().getFont("courier.ttf");
		if(!font) return false;
		for(int i = 0; i < 9; i++)
		{
			Rect* rect = Root::shared().getOverlay()->createRect();
			rects.push_back(rect);
			rects[i]->textureHandle = font->textureHandle;
			rects[i]->offsetInPixels.y = OFFSET_Y;
		}

		float xpos = 0.0f, ypos = 0.0f;
		stbtt_aligned_quad q;
		string s = "FPS = ";

		float lastX = 0.0f;

		for(unsigned int i = 0; i < s.size(); i++)
		{
			stbtt_GetBakedQuad(font->baked, 512, 512, s[i], &xpos ,&ypos,&q,true);
			rects[i]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
			rects[i]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
			rects[i]->offsetInPixels.x = OFFSET_X + lastX;
			rects[i]->sizeInPixels = vec2(q.x1 - q.x0, (q.y1 - q.y0));
			rects[i]->offsetInPixels.y = OFFSET_Y - (rects[i]->sizeInPixels.y/2.0);

			lastX = xpos;
		}

		offsetFPS = lastX;

		// test window
		Window* w = new Window(vec2(0.1f), vec2(0.5f, 0.5f), vec4(1.0, 1.0, 1.0, 1.0));
		makeActive(w);

		// pixel --> screen matrix 

		return true;
	}

	bool Interface::initShaders()
	{
		Shader* overlayVertex = new Shader(VERTEX);
		if(!(overlayVertex->addSourceFile("../shaders/overlay.vert"))) return false;
		if(!(overlayVertex->compile())) return false;

		Shader* overlayFragment = new Shader(FRAGMENT);
		if(!(overlayFragment->addSourceFile("../shaders/overlay.frag"))) return false;
		if(!(overlayFragment->compile())) return false;

		texturedRectProgram = new ShaderProgram("overlay");
		texturedRectProgram->attach(overlayVertex);
		texturedRectProgram->attach(overlayFragment);
		if(!(texturedRectProgram->link())) return false;

		return true;
	}

	void Interface::onFrame(float elapsedTime)
	{	
		Font* font = FontManager::shared().getFont("courier.ttf");
		time += elapsedTime;
		count += 1;

		if (time >= 1.0)
		{
			float xpos = 0.0f, ypos = 0.0f;
			std::stringstream myStream;
			myStream.fill('0');
			myStream.width(3);
			if(count >= 1000) count = 999;
			myStream << count;
			stbtt_aligned_quad q;
			float lastX = 0.0f;
			for(int i = 0; i < 3; i++)
			{
				stbtt_GetBakedQuad(font->baked, 512, 512, myStream.str()[i], &xpos ,&ypos,&q,true);
				rects[i + 6]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
				rects[i + 6]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
				rects[i + 6]->sizeInPixels = vec2(q.x1 - q.x0, (q.y1 - q.y0));
				rects[i + 6]->offsetInPixels.x = lastX + offsetFPS + OFFSET_X;
				rects[i + 6]->offsetInPixels.y = OFFSET_Y - (rects[i + 6]->sizeInPixels.y / 2.0);
				lastX = xpos;
			}
			count = 0;
			time = 0.0;
		}
	}

	void Interface::render()
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		//for(int i = 0; i < windowStack.size(); ++i)
		//	windowStack[i]->draw();

		overlay->render();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

	}

	void Interface::makeActive(Window* w)
	{
		windowStack.push_back(w);
		w->setActiveState(true);
	}

	void Interface::makeInactive(Window* w)
	{
		for(int i = 0; i < windowStack.size(); ++i)
			if(windowStack[i] == w)
				windowStack.erase(windowStack.begin() + i);
	}
}
