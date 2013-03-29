#pragma once

class MapInfo;
class ServerGameSession;

//In the final scenario we have a list of loaded MapInfo's
//Currently we only have one. It is defined in Scripting.cpp for now
extern MapInfo* theMap;

class MapInfo
{
	public:
        MapInfo(int id)
        {
            typeId = id;
            //TODO: add it to the map list
            if(theMap) delete theMap;
            theMap = this;

            maxPlayers = 0;
            width = 0.0f;
            height = 0.0f;
            heightmapSize = 0;
        }

		MapInfo(int id,
                int players,
                float _width, 
				float _height,
				string _name, 
				string _heightmap, 
				int _heightmapSize,
				string _splatmap,
				string _tileset)
		{
            typeId = id;
            //TODO: add it to the map list
            if(theMap) delete theMap;
            theMap = this;

            maxPlayers = players;
			width = _width;
			height = _height;
			name = _name;
			heightmap = _heightmap;
			heightmapSize = _heightmapSize;
			splatmap = _splatmap;
			tileset = _tileset;
		}
		virtual ~MapInfo() { };

        int typeId;

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
        virtual void onLoad(ServerGameSession* serversession) {};
        virtual void onLoadFaction(ServerGameSession* serversession, int factionId, int factionSpawnPos) {};
};
