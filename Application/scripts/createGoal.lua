local entity = scene.CreateEntity()
scene.SetComponent(entity, "goal", false)
scene.SetComponent(entity, "visual", "goal_locked", "goal_locked", true) 
scene.SetComponent(entity, "behaviour", "scripts/goal.lua")

local transform = {
	position = {
		x = 2,
		y = 1,
		z = 0
	},

	rotation = {
		x = 0,
		y = 0,
		z = 0
	},

	scale = {
		x = 3,
		y = 3,
		z = 3
	}
}

scene.SetComponent(entity, "collision", 2)
scene.SetComponent(entity, "transform", transform)
scene.SetComponent(entity, "vector", transform.rotation)
scene.SetComponent(entity, "color", {r = 255, g = 255, b = 255})
scene.SetComponent(entity, "clickable")