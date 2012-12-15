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

        const vector<Unit*>& getUnits() const { return units; }
    private:
        vector<Unit*> units;
};
