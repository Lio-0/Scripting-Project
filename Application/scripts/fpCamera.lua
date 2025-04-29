local playerController = {}
local vector = require("vector")

function playerController:OnCreate()
end

function playerController:OnUpdate(delta)

	local transform = scene.GetComponent(self.ID, "transform")
	local camera = scene.GetComponent(self.ID, "camera")

	local targetDir = vector(camera.targetDir.x, camera.targetDir.y, camera.targetDir.z)
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

--	camera.targetDir.x = camera.targetDir.x * math.cos(lookDeltaX) + camera.targetDir.z * math.sin(lookDeltaX) -- x around y

--	camera.targetDir.z = -camera.targetDir.x * sin(lookDeltaX) + camera.targetDir.z * math.cos(lookDeltaX) -- z around y

--	print(vector(camera.targetDir.x, camera.targetDir.y, camera.targetDir.z))

	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "camera", camera)
end

return playerController

