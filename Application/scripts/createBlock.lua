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
		x = 1,
		y = 1,
		z = 1
	}
}

scene.SetComponent(entity, "visual", "cube", "grey_texture", true) 
scene.SetComponent(entity, "collision", 1)
scene.SetComponent(entity, "transform", transform)
