#include "Mesh.h"
#include "Models.h"

namespace Arya
{
    class Triangle : public Model
    {
        public:
            Triangle();
            ~Triangle(){};
    };

    class Quad : public Model
    {
        public:
            Quad();
            ~Quad(){};
    };
}

