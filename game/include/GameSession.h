#pragma once

#include "Arya.h"
#include "Events.h"

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
using Arya::Shader;
using Arya::ShaderProgram;

class Faction;
class GameSessionInput;

class GameSession : public Arya::FrameListener, public EventHandler
{
    public:
        GameSession();
        ~GameSession();

        bool init();
        bool initShaders();
        bool initVertices();

        Faction* getLocalFaction() const { return localFaction; } ;
        const vector<Faction*>& getFactions() const { return factions; }

        // FrameListener
        void onFrame(float elapsedTime);
        void onRender();

        void handleEvent(Packet& packet);

    private:
        GameSessionInput* input;
        Faction* localFaction;
        vector<Faction*> factions;
        vector<int> clients;

        ShaderProgram* decalProgram;
        GLuint decalVao;

        GLuint selectionDecalHandle;
};
