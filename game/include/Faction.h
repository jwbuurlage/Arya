#include <vector>
using std::vector;

class Unit;

class Faction
{
    public:
        Faction();
        ~Faction();

    private:
        vector<Unit*> units;
};
