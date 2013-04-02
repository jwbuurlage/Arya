local chapel = createUnitType(newUnitTypeId)

chapel.displayname = "Chapel"
chapel.modelname="chapel"
chapel.radius = 100.0
chapel.attackRadius = 0.0
chapel.viewRadius = 200.0
chapel.speed = 0.0
chapel.yawSpeed = 0.0
chapel.maxHealth = 120.0
chapel.damage = 0.0
chapel.attackSpeed = 0.0
chapel.canMoveWhileAttacking = false
chapel.selectionSound = "yes.wav"
chapel.attackSound = "yes.wav"

chapel.onSpawn =
function(unit)
end

chapel.onDeath = 
function(unit)
end

chapel.onDamage = 
function(victim, attacker, damage)
end
