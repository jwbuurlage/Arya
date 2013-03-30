#include "Decals.h"

namespace Arya
{
	Decal::Decal(Texture* _texture, vec2 _pos, vec2 _size)
	{
		texture = _texture;
		pos = _pos;
		size = _size;
	}

	Decal::~Decal()
	{

	}
}
