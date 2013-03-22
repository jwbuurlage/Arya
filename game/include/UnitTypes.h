#pragma once
#include <string>

using std::string;

class Unit;
class UnitInfo;

UnitInfo* getUnitInfo(int type);

void registerNewUnitInfo(UnitInfo* info);

//This class is subclassed by LuaUnitType
//The class instances are created in the scripts
struct UnitInfo
{
    UnitInfo(int type) : typeId(type) { registerNewUnitInfo(this); }

    UnitInfo(int type, const char* name,
            float _radius, float _attackRadius, float _viewRadius,
            float _speed, float _yawSpeed,
            float _maxHealth, float _damage, float _attackSpeed, bool _canMoveWhileAttacking,
            const char* _selectionSound, const char* _attackSound)
        : typeId(type)
    {
        registerNewUnitInfo(this);
        displayname = name;
        modelname = name;
        radius = _radius;
        attackRadius = _attackRadius;
        viewRadius = _viewRadius;
        speed = _speed;
        yawSpeed = _yawSpeed;
        maxHealth = _maxHealth;
        damage = _damage;
        attackSpeed = _attackSpeed;
        canMoveWhileAttacking = _canMoveWhileAttacking;
        selectionSound = _selectionSound;
        attackSound = _attackSound;
    }
    virtual ~UnitInfo(){}

    //Call these from the game
    //They will call the appropriate script
    virtual void onDeath(Unit* unit){};
    virtual void onSpawn(Unit* unit){};
    virtual void onDamage(Unit* victim, Unit* attacker, float damage){};

    const int typeId;

    string displayname;
    string modelname;

    float radius;
    float attackRadius;
    float viewRadius;
    float speed;
    float yawSpeed; //in degrees

    float maxHealth;
    float damage;
    float attackSpeed; //the time one attack takes
    bool canMoveWhileAttacking;

    string selectionSound;
    string attackSound;
};


