local entity = scene.CreateEntity()

local transform = {
	position = {
		x = 2 + 2 * entity,
		y = - 4.5 + 0.5 * entity,
		z = 5
	},

	rotation = {
		x = 0,
		y = 0,
		z = 0
	},

	scale = {
		x = 4,
		y = 0.5,
		z = 4
	}
}

scene.SetComponent(entity, "visual", "cube", "grey_texture", true) 
scene.SetComponent(entity, "collision", 1)
scene.SetComponent(entity, "transform", transform)
