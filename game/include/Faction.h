#include "Arya.h"
#include <list>

using std::list;

using Arya::Object;

class Unit;
class Packet;

class Faction
{
    public:
        Faction();
        ~Faction();

        void addUnit(Unit* unit);

        void setColor(int col) { color = col; }
        vec3 getColor();

        list<Unit*>& getUnits() { return units; }

        void serialize(Packet& pk);
        void deserialize(Packet& pk);

        void setId(int _id){ id = _id; }

    private:
        list<Unit*> units;
        int color;
        int id;
};
