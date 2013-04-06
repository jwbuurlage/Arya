//GameSession is a base class for ClientGameSession and ServerGameSession
//It serves as a Unit and Faction factory class
//Units and Factions are created with createUnit and createFaction
//and they can be deleted by just calling delete on them.
//Every Unit and Faction has a pointer to this class
//so that it can de-register itself at deconstruction
//Note that this subclass does not create or delete the Scripting or Map class
//The Scripting class is created/deleted by Game and or Server
//and all ServerGameSessions share the same Scripting class.
//The Map class is created by the ClientGameSessions subclass and or Server
#pragma once

#include <map>

class Scripting;
class Unit;
class Faction;
class Map;

class GameSession
{
    public:
        GameSession(Scripting* scripting, bool isServer);
        virtual ~GameSession();

        bool isServer() const { return isServerSession; }

        Scripting* getScripting() const { return scripting; }
        Map* getMap() const { return map; }
        float getGameTime() const { return gameTimer; }

        Unit* createUnit(int id, int type);
        Unit* getUnitById(int id);

        Faction* createFaction(int id);
        Faction* getFactionById(int id);
    protected:
        Map* map;

        //Time since start of the game in seconds
        float gameTimer;

    private:
        friend class Unit;
        friend class Faction;
        //We have to use std:: here because we also have a variable called map
        std::map<int,Unit*> unitMap;
        std::map<int,Faction*> factionMap;
        typedef std::map<int,Unit*>::iterator unitMapIterator;
        typedef std::map<int,Faction*>::iterator factionMapIterator;
        void destroyUnit(int id); //called in Unit deconstructor
        void destroyFaction(int id); //called in Faction deconstructor

        Scripting* const scripting;

        //This bool specifies if this session is a server session.
        //It is used by the Unit update loop to decide wether it should
        //send event packets or do nothing.
        bool isServerSession;
};
