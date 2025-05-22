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

entity = scene.CreateEntity()
scene.SetComponent(entity, "visual", "skybox", "skybox_texture", true)
scene.SetComponent(entity, "color", {r = 255, g = 255, b = 255})

transform.position.x = 0
transform.position.y = 0
transform.position.z = 0
transform.rotation.y = 0
transform.rotation.z = 0
transform.rotation.x = 0
transform.scale.x = -1
transform.scale.y = -1
transform.scale.z = -1

scene.SetComponent(entity, "transform", transform)
