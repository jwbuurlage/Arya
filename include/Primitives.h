#include "Mesh.h"
#include "Objects.h"

namespace Arya
{
    class Triangle : public Mesh
    {
        public:
            Triangle();
            ~Triangle();

        private:
            void init();
    };

    class ColoredTriangle : public StaticObject
    {
        public:
            ColoredTriangle();
            ~ColoredTriangle();

        private:
            void makeVAO();
    };
}

