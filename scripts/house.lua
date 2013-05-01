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

house.onUpdate =
function(unit, elapsedTime)
    unit.customData.spawnTimer = (unit.customData.spawnTimer or 0) + elapsedTime
    if unit.customData.spawnTimer > 5 then
        unit.customData.spawnTimer = unit.customData.spawnTimer - 5
        --initialize variable if it doesnt exist, and increase by one
        unit.customData.spawnCounter = (unit.customData.spawnCounter or 0) + 1
        spawnUnit(unit.factionId, 1, vec2(unit.position.x + 60*math.sin(unit.customData.spawnCounter), unit.position.y + 60*math.cos(unit.customData.spawnCounter)))
    end
end
