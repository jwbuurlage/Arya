#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
using glm::vec2;

#include <vector>
using std::vector;

namespace Arya
{
	struct Visionary
	{
		Visionary(vec2* _pos, float* _radius)
		{
			pos = _pos;
			radius = _radius;
		}

		~Visionary()
		{

		}

		vec2* pos;
		float* radius;
	};

	class FogMap
	{
		public:
			FogMap(int _fogMapSize, float _representedSize);
			~FogMap();

			void init();
			void initTexture();
			void updateTexture();
			void clear();
			void blur();
			void colorize(vec2 position, float radius);
			void update();

			bool isVisible(vec2 pos);
			void addVisionary(Visionary* visionary);
			void removeVisionary(Visionary* visionary);

			// TODO: NOT YET IMPLEMENTED
			GLuint getFogMapTextureHandle() const { return fogMapTextureHandle; };

		private:
			int fogMapSize; //pixelcount of one side

			// 1st bit is for 'explored'
			// 2nd bit is for 'visible'
			unsigned char* fogData;

			// not sure what to do with this yet
			unsigned char* fogDeltaData; // TODO: how to use this?

			void precalculateBresenhemCircles();
			unsigned char** bresenhemCircles;

			void positionToIndex(vec2 pos, int& centerX, int& centerY);
			float representedSize;

			vector<Visionary*> visionaries;
			GLuint fogMapTextureHandle;
	};
}

