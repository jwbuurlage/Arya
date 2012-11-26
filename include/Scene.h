#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

namespace Arya
{
    class Scene
    {
        public:
            Scene() { };
            ~Scene() { };

            bool init();
            void render();

        private:
    };
}
