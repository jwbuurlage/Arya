local mill = createUnitType(newUnitTypeId)

mill.displayname = "Mill"
mill.modelname="mill"
mill.radius = 100.0
mill.attackRadius = 0.0
mill.viewRadius = 200.0
mill.speed = 0.0
mill.yawSpeed = 0.0
mill.maxHealth = 120.0
mill.damage = 0.0
mill.attackSpeed = 0.0
mill.canMoveWhileAttacking = false
mill.selectionSound = "yes.wav"
mill.attackSound = "yes.wav"

mill.onSpawn =
function(unit)
end

mill.onDeath = 
function(unit)
end

mill.onDamage = 
function(victim, attacker, damage)
end
