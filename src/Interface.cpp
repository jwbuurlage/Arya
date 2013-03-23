#include "Overlay.h"
#include "Root.h"
#include <iostream>
#include "Interface.h"
#include "Fonts.h"
#include "Textures.h"
#include "Overlay.h"
#include "Shaders.h"
#include "DrawableText.h"
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

	InterfaceElement::InterfaceElement(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size)
	{
		absolutePosition= _absolutePosition;
		relativePosition= _relativePosition;
		size = _size;
		parent = 0;

		recalculateScreenSizeAndPosition();
	}

	void InterfaceElement::setParent(InterfaceElement* _parent)
	{
 		parent = _parent;
		recalculateScreenSizeAndPosition();
	}

	void InterfaceElement::recalculateScreenSizeAndPosition()
	{
		if(parent) {
			vec4 tmpScreenAbsolute = Root::shared().getPixelToScreenTransform() * vec4(absolutePosition, 0.0, 1.0);
			screenOffset = (relativePosition + vec2(1.0)) * (0.5f * parent->getScreenSize()) + vec2(tmpScreenAbsolute.x, tmpScreenAbsolute.y);

			vec4 tmpScreenSize = Root::shared().getPixelToScreenTransform() * vec4(size, 0.0, 1.0);
			screenSize = vec2(tmpScreenSize.x, tmpScreenSize.y);

			screenOffset += parent->getScreenOffset();
		}
		else
		{
			vec4 tmpScreenAbsolute = Root::shared().getPixelToScreenTransform() * vec4(absolutePosition, 0.0, 1.0);
			screenOffset = relativePosition + vec2(tmpScreenAbsolute.x, tmpScreenAbsolute.y);

			vec4 tmpScreenSize = Root::shared().getPixelToScreenTransform() * vec4(size, 0.0, 1.0);
			screenSize = vec2(tmpScreenSize.x, tmpScreenSize.y);
		}
	}

	////////////////////////////////
	// Window
	///////////////////////////////

	Window::Window(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size, vec4 _backgroundColor)
		: InterfaceElement(_relativePosition, _absolutePosition, _size)
	{
		backgroundColor = _backgroundColor;
		isActive = false;
	}

	void Window::addChild(InterfaceElement* ele)
	{
		childElements.push_back(ele);
		ele->setParent(this);
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

		// calculate offset
		// bind shader overlay
		p->use();
		glBindVertexArray(v);

		p->setUniform1i("texture1", 0);
		glActiveTexture(GL_TEXTURE0);
		p->setUniform4fv("uColor", backgroundColor);
		glBindTexture(GL_TEXTURE_2D, TextureManager::shared().getTexture("white")->handle);

		p->setUniform2fv("screenSize", screenSize);
		p->setUniform2fv("screenPosition", screenOffset);
		p->setUniform2fv("texOffset", vec2(0.0f));
		p->setUniform2fv("texSize", vec2(1.0f));

		// draw background
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// draw childs
		for(int i = 0; i < childElements.size(); ++i)
			childElements[i]->draw();
	}

	////////////////////////////////
	// Label
	///////////////////////////////

	Label::Label(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Font* _font, string _text)
		: InterfaceElement(_relativePosition, _absolutePosition, _size)
	{
		dt = new DrawableText(_font, _text);
	}

	void Label::draw()
	{
		// TODO: use at init
		ShaderProgram* p = Interface::shared().getClusterTexturedRectProgram();
		GLuint v = Interface::shared().getOnePxRectVAO();
		p->use();

		p->setUniformMatrix4fv("pixelToScreenTransform", Root::shared().getPixelToScreenTransform());
		p->setUniform2fv("screenOffset", screenOffset);

		// font texture
		p->setUniform1i("texture1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dt->getFont()->textureHandle);

		// vao
		glBindVertexArray(dt->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, dt->getVertexCount());
	}

	void Label::setText(string t)
	{
		// TODO: possibly optimize with dynamic_draw
		if(!dt) return;
		DrawableText* newDt = new DrawableText(dt->getFont(), t);
		delete dt;
		dt = newDt;
	}

	////////////////////////////////
	// Interface
	///////////////////////////////

	Interface::Interface()
	{
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

		// ------------------------
		// TODO: remove test window
		vec2 windowSize = vec2(300.0f, 300.0f);
		Window* w = new Window(vec2(1.0f), -1.0f * windowSize - vec2(20.0f), windowSize, vec4(0.0f, 0.0f, 0.3f, 0.6f));

		Font* f = FontManager::shared().getFont("courier.ttf");
		Label* l = new Label(vec2(-1.0f, 1.0f), vec2(20.0f, -30.0f), vec2(0.5f, 0.1f), f, "This is a test window");
		w->addChild(l);

		FPSLabel = new Label(vec2(1.0f, -1.0f), vec2(-80.0f, 10.0f), vec2(0.0f), f, "FPS: ");
		w->addChild(FPSLabel);

		makeActive(w);
		// TODO: remove test window
		// -----------------------
		
		return true;
	}

	bool Interface::initShaders()
	{
		// singular
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

		// cluster
		Shader* cOverlayVertex = new Shader(VERTEX);
		if(!(cOverlayVertex->addSourceFile("../shaders/overlay_cluster.vert"))) return false;
		if(!(cOverlayVertex->compile())) return false;

		Shader* cOverlayFragment = new Shader(FRAGMENT);
		if(!(cOverlayFragment->addSourceFile("../shaders/overlay_cluster.frag"))) return false;
		if(!(cOverlayFragment->compile())) return false;

		clusterTexturedRectProgram = new ShaderProgram("overlaycluster");
		clusterTexturedRectProgram->attach(cOverlayVertex);
		clusterTexturedRectProgram->attach(cOverlayFragment);
		if(!(clusterTexturedRectProgram->link())) return false;

		return true;
	}

	void Interface::onFrame(float elapsedTime)
	{	
		time += elapsedTime;
		count += 1;

		if (time >= 1.0)
		{

			std::stringstream myStream;
			myStream.fill('0');
			myStream.width(3);
			if(count >= 1000) count = 999;

			myStream << "FPS: " << count;
			FPSLabel->setText(myStream.str());

			count = 0;
			time = 0.0;
		}
	}

	void Interface::render()
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		for(int i = 0; i < windowStack.size(); ++i)
			windowStack[i]->draw();

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
