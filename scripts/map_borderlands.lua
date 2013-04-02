local map = createMap(newMapTypeId)

map.maxPlayers = 4
map.width = 1024.0
map.height = 1024.0
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
    neutralFactionId = createFaction()
    spawnUnit(factionId, "The Boss", vec2(-10,  0))
    spawnUnit(factionId, "The Boss", vec2( 10,  0))
    spawnUnit(factionId, "The Boss", vec2(  0,-10))
    spawnUnit(factionId, "The Boss", vec2(  0, 10))
end

map.onLoadFaction =
function(factionId, factionSpawnPos)
    -- This will give one of the 4 corners
    local baseX = -250 + 500 * (factionSpawnPos%2)
    local baseZ = -250 + 500 * math.floor(factionSpawnPos/2)

    print("Spawning faction " .. factionId .. " at position " .. factionSpawnPos .. ". Coordinates: " .. baseX .. "," .. baseZ)

    -- Spawn the boss
    spawnUnit(factionId, "The Boss", vec2(baseX, baseZ))

    -- Spawn the minions
    for i=0,9 do
        spawnUnit(factionId, "ogros", vec2(baseX + 20*math.floor(i/5) + 20, baseZ + 20*(i%5) - 40))
        spawnUnit(factionId, "hep", vec2(baseX - 20*math.floor(i/5) - 20, baseZ + 20*(i%5) - 40))
    end
end

map.onUpdate =
function(elapsedTimeOrTotalTime)
    --check for units near the center
end
