#include "common/Listeners.h"
#include "Overlay.h"

namespace Arya
{
	class Interface : public FrameListener
	{
		public:
			Interface(){};
			~Interface(){};
			void onFrame(float elapsedTime);
      void init();
		private:
      Rect rect[10];
	};
	
}
