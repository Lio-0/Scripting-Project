local entity


entity = scene.CreateEntity()
scene.SetComponent(entity, "behaviour", "scripts/goalButton.lua")
entity = scene.CreateEntity()
scene.SetComponent(entity, "behaviour", "scripts/platformButton.lua")
entity = scene.CreateEntity()
scene.SetComponent(entity, "behaviour", "scripts/orbButton.lua")
entity = scene.CreateEntity()
scene.SetComponent(entity, "behaviour", "scripts/saveAndExitButton.lua")

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

scene.SetComponent(entity, "transform", transform)

transform.position.y = 1 --Camera height
transform.rotation.z = 1 --Camera look direction
scene.SetComponent(entity, "camera", 0, transform.position, transform.rotation)