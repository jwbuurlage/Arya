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

    private:
        lua_State* luaState;
};
