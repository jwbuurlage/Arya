#include <FTGL/ftgl.h>
#include "common/Listeners.h"


namespace Arya
{
	class Interface : public FrameListener
	{
		public:
			Interface(){};
			~Interface(){};
			void update(float elapsedTime);
		private:
	};
	
}
