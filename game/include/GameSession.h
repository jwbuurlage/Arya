#include <vector>
using std::vector;

class Faction;

class GameSession
{
    public:
        GameSession();
        ~GameSession();

        void start();

    private:
        vector<Faction*> factions;
};
