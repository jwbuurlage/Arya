#include "common/Listeners.h"
#include "Overlay.h"

#include <vector>
using std::vector;

#include <glm/glm.hpp>
using glm::vec2;

namespace Arya
{
    class DrawableText;

    class InterfaceElement
    {
        public:
            InterfaceElement();
            virtual ~InterfaceElement();

            virtual void draw();

        private:
            vec2 position;
            vec2 size;
   };

    class View : public InterfaceElement
    {
        public:
            View();
            ~View();

            void draw();

            void addChild(InterfaceElement* ele);
            void removeChild(InterfaceElement* ele);

        private:
            vector<InterfaceElement*> elements;
    };

    class Label : public InterfaceElement
    {
        public:
            Label() { }
            ~Label() { }

            void draw();

        private:
            DrawableText* dt;
    };

    class Interface : public FrameListener
    {
        public:
            Interface() { offsetFPS = 0.0; time = 0.0; count = 0; }
            virtual ~Interface() {}

            void onFrame(float elapsedTime);
            bool init();

        private:
            float time; 
            int count;
            float offsetFPS;

            vector<Rect*> rects;
    };
}
