#include "common/Listeners.h"
#include "Overlay.h"
#include <GL/glew.h>

#include <vector>
using std::vector;

#include <stack>
using std::stack;

#include <glm/glm.hpp>
using glm::vec2;
using glm::vec4;

namespace Arya
{
    class DrawableText;

    class InterfaceElement
    {
        public:
            InterfaceElement(vec2 _position, vec2 _size);
            virtual ~InterfaceElement() { }

            virtual void draw() { }

        private:
            vec2 position;
            vec2 size;
   };

    class Window : public InterfaceElement
    {
        public:
            Window(vec2 _position, vec2 _size, vec4 _backgroundColor);
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
            Label(vec2 _position, vec2 _size);
            ~Label() { }

            void draw() { }

        private:
            DrawableText* dt;
    };

/////////////////////////
// Interface
/////////////////////////

    class Interface : public FrameListener
    {
        public:
            Interface() { offsetFPS = 0.0; time = 0.0; count = 0; }
            virtual ~Interface() {}

            void onFrame(float elapsedTime);
			void render();
            bool init();
            bool initShaders();

			void makeActive(Window* w);
			void makeInactive(Window* w);

			GLuint getonepxRectVAO() const { return onepxRectVAO; };
			Overlay* getOverlay() const { return overlay; }
			ShaderProgram* getTexturedRectProgram() const { return texturedRectProgram; }
			
        private:
            float time; 
            int count;
            float offsetFPS;

			Overlay* overlay;
			ShaderProgram* texturedRectProgram;
			GLuint onepxRectVAO;

            vector<Rect*> rects;
			stack<Window*> windowStack;
    };
}
