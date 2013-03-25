local ranger = createUnitType(newUnitTypeId)

ranger.displayname = "RangerDude"
ranger.modelname="hep"
ranger.radius = 15.0
ranger.attackRadius = 50.0
ranger.viewRadius = 80.0
ranger.speed = 50.0
ranger.yawSpeed = 360.0
ranger.maxHealth = 60.0
ranger.damage = 10.0
ranger.attackSpeed = 0.7
ranger.canMoveWhileAttacking = false
ranger.selectionSound = "yes.wav"
ranger.attackSound = "yes.wav"

ranger.onSpawn =
function(unit)
    print("rangerdude " .. unit.id .. " has now spawned and will kick your ass")
end

ranger.onDeath = 
function(unit)
    print("oh no ranger " .. unit.id .. " died :(")
end

ranger.onDamage = 
function(victim, attacker, damage)
    print("Ranger " .. victim.id .. " received massive damage (" .. damage .. " teradmg). Only " .. victim.health .. " hp left. Unit " .. attacker.id .. " (" .. attacker.health .. " hp), please be careful in the future")
end
