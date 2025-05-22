local entity = scene.CreateEntity()
scene.SetComponent(entity, "behaviour", "scripts/orb.lua")

local transform = {
	position = {
		x = 0,
		y = 4,
		z = 0
	},

	rotation = {
		x = 0,
		y = 0,
		z = 0
	},

	scale = {
		x = 0.3,
		y = 0.3,
		z = 0.3
	}
}
scene.SetComponent(entity, "visual", "orb", "orb_texture", true) 
scene.SetComponent(entity, "collision", 2)
scene.SetComponent(entity, "transform", transform)
scene.SetComponent(entity, "vector", transform.rotation)
scene.SetComponent(entity, "collectible", 0, false)
scene.SetComponent(entity, "clickable")
scene.SetComponent(entity, "color", {r = 255, g = 255, b = 255})