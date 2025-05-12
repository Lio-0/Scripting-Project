local entity = scene.CreateEntity()

local transform = {
	position = {
		x = math.cos(entity) + 1,
		y = entity * 1.2,
		z = math.sin(entity) + 1
	},

	rotation = {
		x = 0,
		y = 0,
		z = 0
	},

	scale = {
		x = 0.25,
		y = 0.25,
		z = 0.25
	}
}

scene.SetComponent(entity, "visual", "orb", "orb_texture", true) 
scene.SetComponent(entity, "transform", transform)
scene.SetComponent(entity, "behaviour", "orb.lua")
scene.SetComponent(entity, "collision", 1)
