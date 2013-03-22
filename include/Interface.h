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

    class InterfaceElement
    {
        public:
            InterfaceElement(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size);
            virtual ~InterfaceElement() { }

            virtual void draw() { }
			void setParent(InterfaceElement* _parent);
			void recalculateScreenSizeAndPosition();
			vec2 getScreenOffset() const { return screenOffset; }
			vec2 getScreenSize() const { return screenSize; }

        protected:
			vec2 relativePosition;
            vec2 absolutePosition;
            vec2 size;
			InterfaceElement* parent;
   			// for drawing
			vec2 screenSize;
			vec2 screenOffset;
	};


    class Window : public InterfaceElement
    {
        public:
            Window(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size, vec4 _backgroundColor);
            ~Window() { }

            void draw();

            void addChild(InterfaceElement* ele);
            void removeChild(InterfaceElement* ele);

			void setActiveState(bool active);

        private:
            vector<InterfaceElement*> childElements;
			bool isActive;
			vec4 backgroundColor;
    };

    class Label : public InterfaceElement
    {
        public:
			Label(vec2 _relativePosition, vec2 _absolutePosition, vec2 _size,
				Font* _font, string _text);
           ~Label() { }

            void draw();

        private:
            DrawableText* dt;
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
            float offsetFPS;

			Overlay* overlay;
			ShaderProgram* texturedRectProgram;
			ShaderProgram* clusterTexturedRectProgram;
			GLuint onePxRectVAO;

            vector<Rect*> rects;
			vector<Window*> windowStack;
    };
}
