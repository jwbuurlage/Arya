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
