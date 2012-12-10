#include <vector>
using std::vector;

namespace Arya
{
    class Terrain;
    class Camera;
    class Texture;
    class Scene;

    class Map
    {
        public:
            Map(const char* hm, vector<Texture*> ts, Texture* sm);
            ~Map();

            void setTerrain(const char* hm, vector<Texture*> ts, Texture* sm);
            void render(Camera* cam);
            void update(float elapsedTime, Scene* scene);

        private:
            Terrain* terrain;
    };
}
