#pragma once

#include "Arya.h"

#include <vector>
using std::vector;

using Arya::Root;
using Arya::Scene;
using Arya::Object;
using Arya::Model;
using Arya::ModelManager;
using Arya::Camera;
using Arya::Map;
using Arya::Texture;
using Arya::TextureManager;

class Faction;
class GameSessionInput;

class GameSession{
    public:
        GameSession();
        ~GameSession();

        bool init();
        Faction* getLocalFaction() const { return localFaction; } ;

   private:
        GameSessionInput* input;
        Faction* localFaction;
        vector<Faction*> factions;
};
