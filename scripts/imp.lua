local imp = createUnitType(newUnitTypeId)

imp.displayname = "The Boss"
imp.modelname="imp"
imp.radius = 50.0
imp.attackRadius = 15.0
imp.viewRadius = 10.0
imp.speed = 10.0
imp.yawSpeed = 180.0
imp.maxHealth = 300.0
imp.damage = 50.0
imp.attackSpeed = 0.1
imp.canMoveWhileAttacking = false
imp.selectionSound = "chewbacca.wav"
imp.attackSound = "noMercy.wav"

imp.onDamage = 
function(victim, attacker, damage)
    print("STOP ATTACKING THE IMP YOU FAGGOTS. IMP ONLY HAS " .. victim.health .. " HP LEFT!")
end
