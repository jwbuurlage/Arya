#pragma once

class MapInfo
{
	public:
        MapInfo()
        {
            maxPlayers = 0;
            width = 0.0f;
            height = 0.0f;
            heightmapSize = 0;
        }

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
		virtual ~MapInfo() { };

        //amount of spawn positions in the map
        int maxPlayers;

		float width;
		float height;
		string name;

		string heightmap; 
		int heightmapSize;
		string splatmap;
		string tileset; // comma seperated

        //This will call the script callback function
        virtual void onLoad() {};
};
