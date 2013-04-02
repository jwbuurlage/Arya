local house = createUnitType(newUnitTypeId)

house.displayname = "House"
house.modelname="house"
house.radius = 100.0
house.attackRadius = 0.0
house.viewRadius = 200.0
house.speed = 0.0
house.yawSpeed = 0.0
house.maxHealth = 120.0
house.damage = 0.0
house.attackSpeed = 0.0
house.canMoveWhileAttacking = false
house.selectionSound = "yes.wav"
house.attackSound = "yes.wav"

house.onSpawn =
function(unit)
end

house.onDeath = 
function(unit)
end

house.onDamage = 
function(victim, attacker, damage)
end
