#include "common/Listeners.h"
#include "Overlay.h"

namespace Arya
{
    class Interface : public FrameListener
    {
        public:
            Interface(){ offsetFPS = 0.0; time = 0.0; count = 0; }
            ~Interface(){}

            void onFrame(float elapsedTime);
            bool init();

        private:
            float time; 
            int count;
            float offsetFPS;

            vector<Rect*> rects;
    };

}
