#pragma once 

#include "Arya.h"

using Arya::Object;

typedef struct
{
    float radius;
} UnitInfo;

class Unit
{
    public:
        Unit(UnitInfo* inf);
        ~Unit();

        void setObject(Object* obj);
        Object* getObject() const { return object; }

       UnitInfo* getInfo() const { return info; }
       void setInfo(UnitInfo* unitInfo) { info = unitInfo; }
       void setSelected(bool sel) { selected = sel; }

    private:
        Object* object;
        UnitInfo* info;
        bool selected;
};

