#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_truetype.h"
#include <iostream>
#include "Interface.h"
using namespace std;

namespace Arya
{
	float time = 0.0; 
	float count = 0.0;
		
	
	void Interface::update(float elapsedTime)
	{	
		
		time += elapsedTime;
		count += 1;
		
		if (time >= 1.0)
		{
			cout << 1/count << endl;
			count = 0.0;
			time = 0.0;
		}
	}
}
