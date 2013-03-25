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
    private:
        lua_State* luaState;
};
