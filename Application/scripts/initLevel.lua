local entity = scene.CreateEntity()

--Get player starting position from level file
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
			x = 0,
			y = 0,
			z = 0
		}
	}


scene.SetComponent(entity, "behaviour", "scripts/playerController.lua")
scene.SetComponent(entity, "transform", transform)

transform.position.y = 0.5
transform.rotation.z = 1
scene.SetComponent(entity, "camera", 0, transform.position, transform.rotation)