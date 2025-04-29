local entity = scene.CreateEntity()

local transform = {
	position = {
		x = math.random(-10, 10),
		y = 0,
		z = math.random(-10, 10)
	},

	rotation = {
		x = 0,
		y = 0,
		z = 0
	},

	scale = {
		x = 1.0,
		y = 1.0,
		z = 1.0
	}
}

scene.SetComponent(entity, "health", 120)
scene.SetComponent(entity, "transform", transform)
scene.SetComponent(entity, "behaviour", "filur.lua")
scene.SetComponent(entity, "visual", "cube", "", true) 
