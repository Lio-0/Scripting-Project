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

local camera = {
	targetDir = {
		x = 0,
		y = 0,
		z = 1
	},

	rightAxis = {
		x = 1,
		y = 0,
		z = 0
	},

	upAxis = {
		x = 0,
		y = 1,
		z = 0
	}
}

local vector = {
	x = 0,
	y = 0,
	z = 0
}

scene.SetComponent(entity, "vector", vector)
scene.SetComponent(entity, "transform", transform)
scene.SetComponent(entity, "behaviour", "scripts/playerController.lua")

entity = scene.CreateEntity()

scene.SetComponent(entity, "transform", transform)
scene.SetComponent(entity, "camera", camera)
scene.SetComponent(entity, "behaviour", "scripts/fpCamera.lua")