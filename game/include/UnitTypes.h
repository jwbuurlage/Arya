#pragma once
#include <string>

using std::string;

class Unit;
class UnitInfo;

UnitInfo* getUnitInfo(int type);
UnitInfo* getUnitInfo(const std::string& name);

void registerNewUnitInfo(UnitInfo* info);

//This class is subclassed by LuaUnitType
//The class instances are created in the scripts
class UnitInfo
{
	public:
		UnitInfo(int type);
		virtual ~UnitInfo();

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

        string animationIdle;
        string animationMove;
        string animationAttack;
        string animationAttackOutOfRange;
        string animationDie;
};

