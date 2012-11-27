#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

class Object;

namespace Arya
{
    class Scene
    {
        public:
            Scene();
            ~Scene() { };

            bool init();
            void render();

        private:
            vector<Object*> objects;
    };
}
