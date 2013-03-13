#include "../include/UnitTypes.h"

// CONSTRUCTOR HEADER FOR REFERENCE
//UnitInfo(const char* _name, float _radius, float _attackRadius, float _speed, float _yawSpeed,
            //float _maxHealth, float _damage, float _attackSpeed)

UnitInfo infoForUnitType[] = {
            //name      radius  atRad   speed   yawSpeed health   damage  attackSpeed   moveWhileAttacking   selectionSound   attackSound
    UnitInfo("ogros",   10.0f,  5.0f,   30.0f,  720.0f,  100.0f,  20.0f,  1.0f,         true,                "yes.wav",       "yes.wav"),
    UnitInfo("hep",     15.0f,  50.0f,  50.0f,  360.0f,  60.0f,   10.0f,  0.7f,         false,               "yes.wav",       "yes.wav"),
    UnitInfo("imp",     30.0f,  15.0f,  10.0f,  180.0f,  300.0f,   50.0f,  0.1f,        false,               "chewbacca.wav", "noMercy.wav")
};
