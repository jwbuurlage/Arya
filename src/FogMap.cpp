#include "FogMap.h"
#include <string.h>

#include <iostream>
using std::cout;
using std::endl;

#include "common/Logger.h"
#include "Interface.h"
#include "Textures.h"

#define MAX_CIRCLE_SIZE 40
#define UPDATE_TIME 0.1f

namespace Arya
{
	FogMap::FogMap(int _fogMapSize, float _representedSize)
	{
		fogMapSize = _fogMapSize;
		representedSize = _representedSize;

		fogData = 0;
		fogDeltaData = 0;
        bresenhemCircles = 0;
	}

	FogMap::~FogMap()
	{
		if(fogDeltaData)
			delete[] fogDeltaData;
		if(fogData)
			delete[] fogData;

        if(bresenhemCircles)
        {
            for(int i = 0; i < MAX_CIRCLE_SIZE; ++i)
                delete[] bresenhemCircles[i];
            delete[] bresenhemCircles;
        }
	}

	void FogMap::positionToIndex(vec2 pos, int& centerX, int& centerY)
	{
		vec2 shiftedPosition = pos;
		shiftedPosition += vec2(0.5f*representedSize);
		centerX = (int)(fogMapSize * (shiftedPosition.x / representedSize));
		centerY = (int)(fogMapSize * (shiftedPosition.y / representedSize));
	}

	bool FogMap::isVisible(vec2 pos)
	{
		int centerX, centerY;
		positionToIndex(pos, centerX, centerY);
		return fogData[centerY*fogMapSize + centerX] > 63;
	}

	bool FogMap::init()
	{
		fogData = new unsigned char[fogMapSize * fogMapSize];
		fogDeltaData = new unsigned char[fogMapSize * fogMapSize];

		precalculateBresenhemCircles();
		initTexture();
		return true;
	}

	void FogMap::initTexture()
	{
        glGenTextures(1, &fogMapTextureHandle);
		updateTexture();
	}

	void FogMap::updateTexture()
	{
        glBindTexture(GL_TEXTURE_2D, fogMapTextureHandle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, fogMapSize, fogMapSize, 0, GL_RED, GL_UNSIGNED_BYTE, fogDeltaData);
	}

	void FogMap::addVisionary(Visionary* visionary)
	{
		visionaries.push_back(visionary);
	}

	void FogMap::removeVisionary(Visionary* visionary)
	{
		for(int i = 0; i < visionaries.size(); ++i)
			if(visionaries[i] == visionary)
			{
				visionaries.erase(visionaries.begin() + i);
				return;
			}

		LOG_INFO("Deleting visionary that is not registered");
	}

	inline int indexInArray(int x, int y, int r)
	{
		return ((r -y)*(2*r+1) + x + r);
	}

	void FogMap::precalculateBresenhemCircles()
	{
		// we want to save arrays for different sizes
		// of width nxn that represent the circles.. 
		// we then can change the fogmap per line.
		//
		// the ith bresenhemcircle will have size
		// (2*i+1)*(2*i+1)
		// As an example the first bresenhemcircle
		// is given by
		//
		// 0 1 0
		// 1 0 1
		// 0 1 0
		//
		// which is then filled up such that it becomes
		//		
		// 0 1 0
		// 1 1 1
		// 0 1 0
		//
		// etc... 

		bresenhemCircles = new unsigned char*[MAX_CIRCLE_SIZE];

		for(int r = 1; r <= MAX_CIRCLE_SIZE; ++r)
		{
			bresenhemCircles[r-1] = new unsigned char[(2*r+1)*(2*r+1)];
			memset(bresenhemCircles[r-1], 0, sizeof(unsigned char) * (2*r+1)*(2*r+1));
			// fill it
			int x = r;
			int y = 0;
			while(x >= y)
			{
				bresenhemCircles[r-1][indexInArray(x, y, r)] = 1;
				bresenhemCircles[r-1][indexInArray(y, x, r)] = 1;

				bresenhemCircles[r-1][indexInArray(-x, y, r)] = 1;
				bresenhemCircles[r-1][indexInArray(y, -x, r)] = 1;

				bresenhemCircles[r-1][indexInArray(x, -y, r)] = 1;
				bresenhemCircles[r-1][indexInArray(-y, x, r)] = 1;

				bresenhemCircles[r-1][indexInArray(-x, -y, r)] = 1;
				bresenhemCircles[r-1][indexInArray(-y, -x, r)] = 1;

				y++;
				if((2*((x*x + y*y - r*r) + (2*y + 1)) + (1 - 2*x)) > 0)
					x--;
			}

			// scanline
			for(int y = r; y >= -r; --y)
			{
				int p = 0;
				for(int x = -r; x <= 0; ++x)
					if(bresenhemCircles[r-1][indexInArray(x, y, r)] == 1)
					{
						p = x;
						break;
					}

				for(int x = p+1; x < r + (-r-p); ++x)
					bresenhemCircles[r-1][indexInArray(x, y, r)] = 1;
			}

			// for debug: 
			/* cout << "------------ CIRCLE " << r << " -------------" << endl;
			for(int i = 0; i < 2*r + 1; ++i)
			{
				for(int j = 0; j < 2*r + 1; ++j)
					cout << (int)bresenhemCircles[r-1][i*(2*r+1) + j] << " ";
				cout << endl;
			} */
		}
	}

	void FogMap::clear()
	{
		//TODO: THIS IS VERY SLOW AND SUBUPTIMAL
		//memset(fogData, 0, fogMapSize * fogMapSize);
		for(int i = 0; i < fogMapSize * fogMapSize; ++i)
			fogData[i] = fogData[i] > 0 ? 63 : 0;
	}

	void FogMap::blur()
	{
		// 5 px gaussian blur
		int px = 2;
		for(int i = 0; i < fogMapSize; ++i)
			for(int j = 0; j < fogMapSize; ++j)
			{
				int sum = 0;
				int num = 0;
				for(int dx = -px; dx <= px; ++dx)
					for(int dy = -px; dy <= px; ++dy)
					{
						if((i + dy) * fogMapSize + (j + dx) < 0 ||
							(i + dy) * fogMapSize + (j + dx) > fogMapSize * fogMapSize - 1)
							continue;
						num++;
						sum += fogData[(i + dy) * fogMapSize + (j + dx)];
					}
				int average = sum / num;
				fogDeltaData[i * fogMapSize + j] = (char)average;
			}
	}

	void FogMap::update(float elapsedTime)
	{
		fogUpdateTime += elapsedTime;
		if(fogUpdateTime < UPDATE_TIME)
			return;
		fogUpdateTime -= UPDATE_TIME;
		clear();
		for(int i = 0; i < visionaries.size(); ++i)
			colorize(*visionaries[i]->pos, *visionaries[i]->radius);

		blur();
		updateTexture();
	}

	void FogMap::colorize(vec2 position, float radius)
	{
		int centerX, centerY;
		positionToIndex(position, centerX, centerY);
		int fogRadius = (int)(fogMapSize * (radius / representedSize));

		if(fogRadius > MAX_CIRCLE_SIZE)
		{
			LOG_WARNING("Radius not precalculated for r = " << fogRadius << " max value is " << MAX_CIRCLE_SIZE);
			fogRadius = MAX_CIRCLE_SIZE;
		}

		// update fog map per line
		for(int j = centerY - fogRadius; j < centerY + fogRadius; ++j)
		{
			if(j < 0 || j >= fogMapSize) continue;
			for(int i = centerX - fogRadius; i < centerX + fogRadius; ++i)
			{
				if(i < 0 || i >= fogMapSize) continue;
				if(bresenhemCircles[fogRadius-1][indexInArray(i - centerX, j - centerY, fogRadius)])
					fogData[j*fogMapSize + i] = 255;
//				fogData[j*fogMapSize + i] |= 
//					bresenhemCircles[fogRadius-1][indexInArray(i - centerX, j - centerY, fogRadius)];
			}
		}
	}
}
