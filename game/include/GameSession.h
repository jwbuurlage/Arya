//GameSession is a base class for ClientGameSession and ServerGameSession
//It serves as a Unit and Faction factory class
//Units and Factions are created with createUnit and createFaction
//and they can be deleted by just calling delete on them.
//Every Unit and Faction has a pointer to this class
//so that it can de-register itself at deconstruction
//Note that the GameSession does not create or delete the scripting class
//The Scripting class is created/deleted by Game and or Server
//and all ServerGameSessions share the same Scripting class.
#pragma once

#include <map>
using std::map;

class Scripting;
class Unit;
class Faction;

class GameSession
{
    public:
        GameSession(Scripting* scripting);
        virtual ~GameSession();

        Scripting* getScripting() const { return scripting; }

        Unit* createUnit(int id, int type);
        Unit* getUnitById(int id);

        Faction* createFaction(int id);
        Faction* getFactionById(int id);
    private:
        friend class Unit;
        friend class Faction;
        map<int,Unit*> unitMap;
        map<int,Faction*> factionMap;
        typedef map<int,Unit*>::iterator unitMapIterator;
        typedef map<int,Faction*>::iterator factionMapIterator;
        void destroyUnit(int id); //called in Unit deconstructor
        void destroyFaction(int id); //called in Faction deconstructor

        Scripting* const scripting;
};
