#include "Arya.h"
#include <list>
using std::list;

using Arya::Object;

class Unit;

class Faction
{
    public:
        Faction();
        ~Faction();

        void addUnit(Unit* unit);

        vec3 getColor() const { return color; }
        void setColor(vec3 col) { color = col; }

        list<Unit*>& getUnits() { return units; }
    private:
        list<Unit*> units;
        vec3 color;
};
