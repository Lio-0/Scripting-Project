local playerController = {}
local vector = require("vector")

function playerController:OnCreate()
end

function playerController:OnUpdate(delta)

	local transform = scene.GetComponent(self.ID, "transform")
	local camera = scene.GetComponent(self.ID, "camera")

	local lookDeltaX, lookDeltaY = input.GetMouseDelta()

	if input.IsKeyDown(87) then -- W
		transform.position.x = transform.position.x + delta * 100
	end
	if input.IsKeyDown(83) then -- S
		transform.position.x = transform.position.x - delta * 100
	end
	if input.IsKeyDown(68) then -- A
		transform.position.z = transform.position.z + delta * 100
	end
	if input.IsKeyDown(65) then -- D
		transform.position.z = transform.position.z - delta * 100
	end
	if input.IsKeyPressed(32) then -- Space
		transform.position.y = 10
	end

	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "camera", camera)
end

return playerController

