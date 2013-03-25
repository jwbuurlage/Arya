#pragma once

struct lua_State;

class Scripting
{
    public:
        Scripting();
        ~Scripting();

        int init();
        void cleanup();

        int execute(const char* filename);

        lua_State* getState() const { return luaState; }

        static Scripting& shared() { return *singleton; }
    private:
        static Scripting* singleton;
        lua_State* luaState;
};
