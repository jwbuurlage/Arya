#pragma once

class MapInfo
{
	public:
		MapInfo(float _width, 
				float _height,
				string _name, 
				string _heightmap, 
				int _heightmapSize,
				string _splatmap,
				string _tileset)
		{
			width = _width;
			height = _height;
			name = _name;
			heightmap = _heightmap;
			heightmapSize = _heightmapSize;
			splatmap = _splatmap;
			tileset = _tileset;
		}
		~MapInfo() { };

		float width;
		float height;
		string name;

		string heightmap; 
		int heightmapSize;
		string splatmap;
		string tileset; // comma seperated

		//virtual void onLoad();
};
