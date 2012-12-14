#pragma once 

#include "Arya.h"

using Arya::Object;

class Unit
{
    public:
        Unit();
        ~Unit();

        void setObject(Object* obj);
        Object* getObject() const { return object; }

        void setSelected(bool sel) { selected = sel; };

    private:
        Object* object;
        bool selected;
};

