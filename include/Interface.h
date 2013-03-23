///////////////////////////////////////////////////////////////////////////////////////////////
// Interface, important concepts:
// - Relative position is a vec2 in [-1, 1]^2 that serves as a resolution-independant
// 	 anchor point
// - Absolute position is the offset in pixels from the relative position
// - A window deletes its childs but is not deleted when closed (just made inactive)
//   the allocator is responsible for removing a window but not its childs (i.e. labels etc.)
// - Window flags are meant to be used as in WINDOW_DRAGGABLE | WINDOW_RESIZABLE
///////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "common/Listeners.h"
#include "common/Singleton.h"
#include "Overlay.h"
#include <GL/glew.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <glm/glm.hpp>
using glm::vec2;
using glm::vec4;
using glm::mat4;

namespace Arya
{
    class DrawableText;
    class Font;
	class Texture;

	/////////////////////
	// Delegate protocols
	/////////////////////

	class Button;
	class ButtonDelegate
	{
		public:
			ButtonDelegate() { }
			virtual ~ButtonDelegate() { }

			virtual void buttonClicked(Button* sender) { };
			virtual void buttonDragMoved(Button* sender, float dx, float dy) { }; // dx, dy, in pixels
	};



    class InterfaceElement
    {
        public:
            InterfaceElement(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
					vec4 _colorMask = vec4(1.0f));
            virtual ~InterfaceElement() { }

            virtual void draw() { }

			void setParent(InterfaceElement* _parent);
			void recalculateScreenSizeAndPosition();
			vec2 getScreenOffset() const { return screenOffset; }
			vec2 getScreenSize() const { return screenSize; }
			vec2 getSize() const { return size; }

			void setAbsolutePosition(vec2 _absolutePosition) { absolutePosition = _absolutePosition; recalculateScreenSizeAndPosition(); }
			void setSize(vec2 _size) { size = _size; recalculateScreenSizeAndPosition(); }

            void addChild(InterfaceElement* ele);
            void removeChild(InterfaceElement* ele);

        protected:
			vec2 relativePosition;
            vec2 absolutePosition;
            vec2 size;
			InterfaceElement* parent;
   			// for drawing
			vec2 screenSize;
			vec2 screenOffset;
			vec4 colorMask; // i.e. vec4(1.0, 0.0, 0.0, 1.0) is blue filter

            vector<InterfaceElement*> childElements;
	};
	
	typedef enum
	{
		WINDOW_DRAGGABLE = 1 << 0,
		WINDOW_RESIZABLE = 1 << 1,
		WINDOW_CLOSABLE  = 1 << 2,
	} WindowFlags;

	class Image;
    class Window : public InterfaceElement, public ButtonDelegate
    {
        public:
            Window(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Texture* _backgroundTexture, int flags, string _title,
				vec4 _backgroundColor = vec4(1.0));
            ~Window();

            void draw();

			void setActiveState(bool active);
			void setSize(vec2 _size);

			// ButtonDelegate
			void buttonClicked(Button* sender);
			void buttonDragMoved(Button* sender, float dx, float dy);

        private:
			Button* titleButton;
			Image* background;
			string title;
			bool isActive;
			int flags;
    };

	// use this for backgrounds
	// uses the whole texture
	class Image : public InterfaceElement
	{
		public:
			Image(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Texture* _texture, vec4 _colorMask = vec4(1.0));
			~Image() { };

			void draw();

		private:
			Texture* texture;
	};

    class Label : public InterfaceElement
    {
        public:
			Label(vec2 _relativePosition, vec2 _absolutePosition,
				Font* _font, string _text, vec4 _colorMask = vec4(1.0));
           ~Label() { }

            void draw();
			void setText(string _t);

        private:
            DrawableText* dt;
    };

	class Button : public InterfaceElement, public InputListener
	{
		public:
			Button(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Texture* _backgroundTexture, Font* _font, string _text, string _identifier,
				ButtonDelegate* _target, bool _draggable, vec4 _colorMask = vec4(1.0));
			~Button() { }

			void draw();

			void setText();
			string getIdentifier() const { return identifier; }

			bool mouseDown(MOUSEBUTTON button, bool buttonDown, int x, int y);
			bool mouseMoved(int x, int y, int dx, int dy);

			bool pointIsInside(vec2 p);
			void setSize(vec2 _size);

		private:
			bool draggable;
			bool dragging;
			Label* label;
			Image* background;
			ButtonDelegate* target;
			string identifier;
	};

/////////////////////////
// Interface
/////////////////////////

    class Interface : public FrameListener, public Singleton<Interface>
    {
        public:
            Interface();
            virtual ~Interface() {}

            void onFrame(float elapsedTime);
			void render();
            bool init();
            bool initShaders();

			void makeActive(Window* w);
			void makeInactive(Window* w);

			GLuint getOnePxRectVAO() const { return onePxRectVAO; };
			Overlay* getOverlay() const { return overlay; }
			ShaderProgram* getTexturedRectProgram() const { return texturedRectProgram; }
			ShaderProgram* getClusterTexturedRectProgram() const { return clusterTexturedRectProgram; }
			
        private:
            float time; 
            int count;

			Label* FPSLabel;
			Overlay* overlay;
			ShaderProgram* texturedRectProgram;
			ShaderProgram* clusterTexturedRectProgram;
			GLuint onePxRectVAO;

            vector<Rect*> rects;
			vector<Window*> windowStack;
    };
}
