local imp = createUnitType(newUnitTypeId)

imp.displayname = "The Boss"
imp.radius = 25.0
imp.attackRadius = 15.0
imp.viewRadius = 10.0
imp.speed = 20.0
imp.yawSpeed = 180.0
imp.maxHealth = 300.0
imp.damage = 50.0
imp.attackSpeed = 1.0
imp.canMoveWhileAttacking = false
imp.selectionSound = "chewbacca.wav"
imp.attackSound = "noMercy.wav"

imp.modelname="imp"
imp.animationIdle = "stand"
imp.animationMove = "walk"
imp.animationAttack = "atk"
imp.animationAttackOutOfRange = "atk"
imp.animationDie = "die"

imp.onDamage = 
function(victim, attacker, damage)
    --print("STOP ATTACKING THE IMP YOU FAGGOTS. IMP ONLY HAS " .. victim.health .. " HP LEFT!")
end
