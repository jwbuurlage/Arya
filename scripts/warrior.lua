local warrior = createUnitType(newUnitTypeId)

warrior.displayname = "Ogre of Doom"
warrior.modelname="ogros"
warrior.radius = 10.0
warrior.attackRadius = 5.0
warrior.viewRadius = 50.0
warrior.speed = 30.0
warrior.yawSpeed = 720.0
warrior.maxHealth = 100.0
warrior.damage = 20.0
warrior.attackSpeed = 1.0
warrior.canMoveWhileAttacking = true
warrior.selectionSound = "yes.wav"
warrior.attackSound = "yes.wav"

warrior.onSpawn = 
function(unit)
    print("all hail warrior " .. unit.id .. " who has now spawned")
end

warrior.onDeath = 
function(unit)
    print("oh no unit " .. unit.id .. " died :(")
end

warrior.onDamage = 
function(victim, attacker, damage)
    print("Ogre " .. victim.id .. " received massive damage (" .. damage .. " teradmg). Only " .. victim.health .. " hp left. Unit " .. attacker.id .. " (" .. attacker.health .. " hp), please be careful in the future")
end

print("warrior lua script reporting for motherfucking duty")
