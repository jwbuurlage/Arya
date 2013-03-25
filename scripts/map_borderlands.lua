local map_borderlands = createMap()

map.width = 1024.0
map.height = 1024.0
map.name = "Borderlands"
map.heightmap = "borderlands_heightmap.raw"
map.heightmapSize = 1025
map.splatmap = "borderlands_splatmap.tga"
map.tileset = "grass.tga,rock.tga,dirt.tga,snow.tga"

map.onLoad =
function(herp, derp, herp)
	print("totally loading up in here")
end
