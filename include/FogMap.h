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

		~Visionary() { }

		vec2* pos;
		float* radius;
	};

	class FogMap
	{
		public:
			FogMap(int _fogMapSize, float _representedSize);
			~FogMap();

			bool init();
			void initTexture();
			void updateTexture();
			void clear();
			void blur();
			void colorize(vec2 position, float radius);
			void update(float elapsedTime);

			// This returns whether a certain position is visible by the local player
			bool isVisible(vec2 pos);
			GLuint getFogMapTextureHandle() const { return fogMapTextureHandle; };

			void addVisionary(Visionary* visionary);
			void removeVisionary(Visionary* visionary);

		private:
			int fogMapSize; //pixelcount of one side

			float fogUpdateTime;

			// as it is now, 255 means visible and 67 means explored
			// maybe later:
			// 1st bit is for 'explored'
			// 2nd bit is for 'visible'
			unsigned char* fogData;

			// not sure what to do with this yet
			// currently use it to store the blurred texture data
			unsigned char* fogDeltaData; // TODO: how to use this?

			void precalculateBresenhemCircles();
			unsigned char** bresenhemCircles;

			void positionToIndex(vec2 pos, int& centerX, int& centerY);
			float representedSize;

			vector<Visionary*> visionaries;
			GLuint fogMapTextureHandle;
	};
}

