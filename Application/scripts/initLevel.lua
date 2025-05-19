local entity = scene.CreateEntity()

scene.SetComponent(entity, "behaviour", "scripts/playerController.lua")

entity = scene.CreateEntity()
local transform = {
	position = {
		x = 0,
		y = 1,
		z = 0
	},

	rotation = {
		x = 0,
		y = 0,
		z = 0
	},

	scale = {
		x = 0.3,
		y = 1,
		z = 0.3
	}
}

local vector = {
	x = 0,
	y = 0,
	z = 0
}

scene.SetComponent(self.ID, "transform", transform)

transform.position.y = 1 --Camera height
transform.rotation.z = 1 --Camera look direction
scene.SetComponent(entity, "camera", 1, transform.position, transform.rotation)
