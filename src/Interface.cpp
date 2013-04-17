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

#define TITLE_BAR_HEIGHT 15.0f

namespace Arya
{
    template<> Interface* Singleton<Interface>::singleton = 0;

	////////////////////////////////
	// InterfaceElement
	///////////////////////////////

	InterfaceElement::InterfaceElement(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
			vec4 _colorMask)
	{
		absolutePosition= _absolutePosition;
		relativePosition= _relativePosition;
		size = _size;
		colorMask = _colorMask;

		parent = 0;

		recalculateScreenSizeAndPosition();
	}

	InterfaceElement::~InterfaceElement()
	{
		for(int i = 0; i < childElements.size(); ++i)
			delete childElements[i];
	}

	void InterfaceElement::addChild(InterfaceElement* ele)
	{
		childElements.push_back(ele);
		ele->setParent(this);
	}

	void InterfaceElement::removeChild(InterfaceElement* ele)
	{
		for(int i = 0; i < childElements.size(); ++i)
		{
			if(childElements[i] == ele)
				childElements.erase(childElements.begin() + i);
		}
	}

	void InterfaceElement::setParent(InterfaceElement* _parent)
	{
 		parent = _parent;
		recalculateScreenSizeAndPosition();
	}	

	void InterfaceElement::recalculateScreenSizeAndPosition()
	{
		if(parent) {
			vec4 tmpScreenAbsolute = Root::shared().getPixelToScreenTransform() * vec4(absolutePosition, 0.0f, 1.0f);
			screenOffset = (relativePosition + vec2(1.0f)) * (0.5f * parent->getScreenSize()) + vec2(tmpScreenAbsolute.x, tmpScreenAbsolute.y);

			vec4 tmpScreenSize = Root::shared().getPixelToScreenTransform() * vec4(size, 0.0, 1.0f);
			screenSize = vec2(tmpScreenSize.x, tmpScreenSize.y);

			screenOffset += parent->getScreenOffset();
		}
		else
		{
			vec4 tmpScreenAbsolute = Root::shared().getPixelToScreenTransform() * vec4(absolutePosition, 0.0, 1.0f);
			screenOffset = relativePosition + vec2(tmpScreenAbsolute.x, tmpScreenAbsolute.y);

			vec4 tmpScreenSize = Root::shared().getPixelToScreenTransform() * vec4(size, 0.0, 1.0f);
			screenSize = vec2(tmpScreenSize.x, tmpScreenSize.y);
		}

		for(int i = 0; i < childElements.size(); ++i)
			childElements[i]->recalculateScreenSizeAndPosition();
	}

	////////////////////////////////
	// Window
	///////////////////////////////

	Window::Window(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
		   	Texture* _backgroundTexture, int _flags, string _title,
			vec4 _backgroundColor)
		: InterfaceElement(_relativePosition, _absolutePosition, _size, _backgroundColor)
	{
		background = new Image(vec2(-1.0f), vec2(0.0f), _size,
				_backgroundTexture, _backgroundColor);
		addChild(background);
		title = _title;
		flags = _flags;

		if(flags & WINDOW_DRAGGABLE)
		{
			// add title button
			vec2 titleButtonSize = vec2(_size.x - ((flags & WINDOW_CLOSABLE) ? TITLE_BAR_HEIGHT : 0.0f), TITLE_BAR_HEIGHT);
			titleButton = new Button(vec2(-1.0f, 1.0f), vec2(0.0f, -titleButtonSize.y), titleButtonSize,
					TextureManager::shared().getTexture("white"), FontManager::shared().getFont("DejaVuSans-Bold.ttf"), _title, "titleButton",
					this, true, vec4(0.0f, 0.0f, 0.0f, 1.0f));
			addChild(titleButton);
		}

		if(flags & WINDOW_CLOSABLE)
		{
			// add close button
			vec2 closeButtonSize = vec2(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
			Button* closeButton = new Button(vec2(1.0, 1.0), -closeButtonSize, closeButtonSize,
					TextureManager::shared().getTexture("white"), FontManager::shared().getFont("DejaVuSans-Bold.ttf"), "x", "closeButton",
					this, false, vec4(0.2f, 0.2f, 0.2f, 1.0f));
			addChild(closeButton);
		}

		if(flags & WINDOW_RESIZABLE)
		{
			// add resize button
			vec2 resizeButtonSize = vec2(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
			Button* resizeButton = new Button(vec2(1.0, -1.0), vec2(-resizeButtonSize.x, 0.0f), resizeButtonSize,
					TextureManager::shared().getTexture("white"), FontManager::shared().getFont("DejaVuSans-Bold.ttf"), ".", "resizeButton",
					this, true, vec4(0.2f, 0.2f, 0.2f, 1.0f));
			addChild(resizeButton);
		}

		isActive = false;
	}

	Window::~Window()
	{
		if(isActive)
			Interface::shared().makeInactive(this);
	}

	void Window::setActiveState(bool active)
	{
		isActive = active;
		//disable buttons..
		for(unsigned int i = 0; i < childElements.size(); ++i)
			childElements[i]->setActiveState(active);
	}

	void Window::draw()
	{
		if(!isActive) return;

		// draw childs
		for(int i = 0; i < childElements.size(); ++i)
			childElements[i]->draw();
	}

	// Window ButtonDelegate
	
	void Window::buttonClicked(Button* sender)
	{
		if(sender->getIdentifier() == "closeButton")
		{
			Interface::shared().makeInactive(this);
		}
	}

	void Window::buttonDragMoved(Button* sender, float dx, float dy)
	{
		if(sender->getIdentifier() == "resizeButton")
		{
			setSize(size + vec2(dx, -dy));
			setAbsolutePosition(absolutePosition + vec2(0, dy));
		}
		else if(sender->getIdentifier() == "titleButton")
		{
			setAbsolutePosition(absolutePosition + vec2(dx, dy));
		}
	}

	void Window::setSize(vec2 _size)
	{
		size = _size;
		recalculateScreenSizeAndPosition();
		background->setSize(_size);

		if(titleButton)
			titleButton->setSize(vec2(_size.x - ((flags & WINDOW_CLOSABLE) ? TITLE_BAR_HEIGHT : 0.0f), TITLE_BAR_HEIGHT));
	}

	////////////////////////////////
	// Image
	///////////////////////////////

	Image::Image(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Texture* _texture, vec4 _colorMask)
		: InterfaceElement(_relativePosition, _absolutePosition, _size, _colorMask)
	{
		texture = _texture;
	}

	void Image::draw()
	{
		ShaderProgram* p = Interface::shared().getTexturedRectProgram();
		GLuint v = Interface::shared().getOnePxRectVAO();

		p->use();
		glBindVertexArray(v);

		p->setUniform1i("texture1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->handle);
		p->setUniform4fv("uColor", colorMask);

		p->setUniform2fv("screenSize", screenSize);
		p->setUniform2fv("screenPosition", screenOffset);
		p->setUniform2fv("texOffset", vec2(0.0f));
		p->setUniform2fv("texSize", vec2(1.0f));

		// draw background
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	////////////////////////////////
	// Label
	///////////////////////////////

	Label::Label(vec2 _relativePosition, vec2 _absolutePosition,
				Font* _font, string _text, vec4 _colorMask)
		: InterfaceElement(_relativePosition, _absolutePosition, vec2(0.0f), _colorMask)
	{
		dt = new DrawableText(_font, _text);
		size = dt->getSize();
	}

	void Label::draw()
	{
		ShaderProgram* p = Interface::shared().getClusterTexturedRectProgram();
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
		// TODO: possibly optimize with dynamic draw
		if(!dt) return;
		DrawableText* newDt = new DrawableText(dt->getFont(), t);
		delete dt;
		dt = newDt;
		size = dt->getSize();
	}

	////////////////////////////////
	// Button
	///////////////////////////////

	Button::Button(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Texture* _backgroundTexture, Font* _font, string _text, string _identifier,
				ButtonDelegate* _target, bool _draggable, vec4 _colorMask)
		: InterfaceElement(_relativePosition, _absolutePosition, _size, _colorMask)
	{
		if(_backgroundTexture)
		{
			background = new Image(vec2(-1.0f), vec2(0.0), _size,
					_backgroundTexture, _colorMask);
			addChild(background);
		}
		if(_text != "")
		{
			label = new Label(vec2(0.0f), vec2(0.0f),
					_font, _text, _colorMask);
			label->setAbsolutePosition(-0.5f*label->getSize());
			addChild(label);
		}

		identifier = _identifier;
		target = _target;
		draggable = _draggable;
		dragging = false;

		// register to input events
		Root::shared().addInputListener(this);
	}

	void Button::draw()
	{
		if(background)
			background->draw();
		if(label)
			label->draw();
	}

	void Button::setSize(vec2 _size)
	{
		size = _size;
		background->setSize(_size);
		recalculateScreenSizeAndPosition();
	}

	bool Button::mouseDown(MOUSEBUTTON button, bool buttonDown, int x, int y)
	{
		if(button == BUTTON_LEFT) {
			if(pointIsInside(vec2(x, y))) {
				if(buttonDown) {
					dragging = true;
					return true;
				} else {
					if(dragging) {
						dragging = false;
						target->buttonClicked(this);
						return true;
					}
				}
			} else {
				dragging = false;
				return false;
			}
		}

		return false;
	}

	bool Button::mouseMoved(int x, int y, int dx, int dy)
	{
		if(!dragging || !draggable)
			return false;

		target->buttonDragMoved(this, (float)dx, (float)dy);
		return true;
	}

	bool Button::pointIsInside(vec2 p)
	{
		// the matrix maps [0,width] x [0,height] --> [0, 2] x [0, 2] so we subtract one
		vec4 pScreen = Root::shared().getPixelToScreenTransform() * vec4(p, 0.0f, 1.0f) - vec4(1.0f, 1.0f, 0.0f, 0.0f);
		if((pScreen.x > screenOffset.x && pScreen.x < screenOffset.x + screenSize.x) &&
			(pScreen.y > screenOffset.y && pScreen.y < screenOffset.y + screenSize.y))
			return true;
		return false;
	}

	void Button::setActiveState(bool active)
	{
		if(isActive == active) return;
		isActive = active;
		if(isActive)
			Root::shared().addInputListener(this);
		else
			Root::shared().removeInputListener(this);
	}

	////////////////////////////////
	// Interface
	///////////////////////////////

	Interface::Interface()
	{
		time = 0.0;
		count = 0;
		overlay = 0; 

        fpsWindow = 0;

        vbo = 0;
        onePxRectVAO = 0;
	}

	Interface::~Interface()
	{
		if(overlay)
			delete overlay;

        if(vbo)
            glDeleteBuffers(1, &vbo);
		if(onePxRectVAO)
			glDeleteVertexArrays(1, &onePxRectVAO);

        if(fpsWindow)
            delete fpsWindow;

		for(int i = 0; i < windowStack.size(); ++i)
			delete windowStack[i];

		if(texturedRectProgram)
			delete texturedRectProgram;
		if(clusterTexturedRectProgram)
			delete clusterTexturedRectProgram;
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

		vec2 windowSize = vec2(100.0f, 100.0f);
		fpsWindow = new Window(vec2(1.0f), -1.0f * windowSize - vec2(20.0f), windowSize, 
				TextureManager::shared().getTexture("white"),  
				0, "",
				vec4(0.0f, 0.0f, 0.0f, 0.0f));

		Font* f = FontManager::shared().getFont("DejaVuSans.ttf");

		FPSLabel = new Label(vec2(1.0f, 1.0f), vec2(-80.0f, -30.0f), f, "FPS: ");
		fpsWindow->addChild(FPSLabel);

		makeActive(fpsWindow);
		
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

		if (time >= 1.0f)
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
        //The window could be resized inbetween
        w->recalculateScreenSizeAndPosition();
	}

	void Interface::makeInactive(Window* w)
	{
		for(int i = 0; i < windowStack.size(); ++i)
			if(windowStack[i] == w)
				windowStack.erase(windowStack.begin() + i);
		w->setActiveState(false);
	}

    void Interface::recalculatePositions()
    {
        for(unsigned int i = 0; i < windowStack.size(); ++i)
            windowStack[i]->recalculateScreenSizeAndPosition();
    }
}
