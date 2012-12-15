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
       bool isSelected() { return selected; }

       void update(float elapsedTime);

       void setTargetPosition(vec2 target);

       void setIdle(bool idl);

    private:
        Object* object;
        UnitInfo* info;
        bool selected;

        // movement stuff
        vec2 targetPosition;
        bool idle;

        //TODO: move these to UnitInfo
        float speed;
        float yawspeed; //in degrees
};
