#include "common/Listeners.h"
#include "Overlay.h"

namespace Arya
{
    class Interface : public FrameListener
    {
        public:
            Interface(){ time = 0.0; count = 0; }
            ~Interface(){}

            void onFrame(float elapsedTime);
            void init();

        private:
            float time; 
            int count;

            vector<Rect*> rects;
    };

}
