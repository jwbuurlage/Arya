local map = createMap(newMapTypeId)

map.maxPlayers = 4
map.width = 2000.0
map.height = 2000.0
map.name = "Borderlands"
map.heightmap = "borderlands_heightmap.raw"
map.heightmapSize = 1025
map.splatmap = "borderlands_splatmap.tga"
map.tileset = "grass.tga,rock.tga,dirt.tga,snow.tga"

map.onLoad =
function()
	print("totally loading up these borders in those lands")
end

map.onLoad =
function()
    print("Spawning neutral unit!")
    spawnTimer = 0
    spawnCounter = 0
    neutralFactionId = createFaction()
    spawnUnit(neutralFactionId, "The Boss", vec2(-15,  0))
    spawnUnit(neutralFactionId, "The Boss", vec2( 15,  0))
    spawnUnit(neutralFactionId, "The Boss", vec2(  0,-15))
    spawnUnit(neutralFactionId, "The Boss", vec2(  0, 15))
end

map.onLoadFaction =
function(factionId, factionSpawnPos)
    -- This will give one of the 4 corners
    local baseX = -250 + 500 * (factionSpawnPos%2)
    local baseZ = -250 + 500 * math.floor(factionSpawnPos/2)

    print("Spawning faction " .. factionId .. " at position " .. factionSpawnPos .. ". Coordinates: " .. baseX .. "," .. baseZ)

    spawnUnit(factionId, "Mill", vec2(baseX + 100, baseZ))
    spawnUnit(factionId, "House", vec2(baseX - 100, baseZ))
    -- spawnUnit(factionId, "Chapel", vec2(baseX, baseZ + 100))

    -- Spawn the boss
    spawnUnit(factionId, "The Boss", vec2(baseX, baseZ))

    -- Spawn the minions
    for i=0,9 do
        spawnUnit(factionId, "ogros", vec2(baseX + 20*math.floor(i/5) + 20, baseZ + 20*(i%5) - 40))
        spawnUnit(factionId, "hep", vec2(baseX - 20*math.floor(i/5) - 20, baseZ + 20*(i%5) - 40))
    end
end

map.onUpdate =
function(elapsedTime)
    spawnTimer = spawnTimer + elapsedTime
    if spawnTimer > 3 then
        spawnTimer = spawnTimer - 3
        spawnUnit(neutralFactionId, "ogros", vec2(40*math.sin(spawnCounter), 40*math.cos(spawnCounter)))
        spawnCounter = spawnCounter + 1
    end
    --check for units near the center
    --local unitList = getUnitsNearLocation(vec2(0,0), 30)
    --for unit in unitList do
    --  checkToWhichFactionTheyBelong
    --end
end
