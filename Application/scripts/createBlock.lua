local entity = scene.CreateEntity()

local transform = {
	position = {
		x = 0,
		y = 2,
		z = 0
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
scene.SetComponent(entity, "clickable")
scene.SetComponent(entity, "color", {r = 255, g = 255, b = 255})
