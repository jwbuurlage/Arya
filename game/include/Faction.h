#include "Arya.h"
#include <vector>
using std::vector;

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

        const vector<Unit*>& getUnits() const { return units; }
    private:
        vector<Unit*> units;
        vec3 color;
};
