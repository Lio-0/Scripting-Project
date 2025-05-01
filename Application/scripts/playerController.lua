local playerController = {}

local vector = require("vector")
local grounded

local function bton(value)
 return value == true and 1 or value == false and 0
end

function playerController:OnCreate()
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

	local vector = {
		x = 0,
		y = 0,
		z = 0
	}

	scene.SetComponent(self.ID, "vector", vector)

	vector.y = 0

	scene.SetComponent(self.ID, "camera", 0, vector)
	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "camera", tranform)
end

function playerController:OnUpdate(delta)
	local transform = scene.GetComponent(self.ID, "transform")
	local velocity = scene.GetComponent(self.ID, "vector")
	local camera = scene.GetComponent(self.ID, "camera")

	local viewDirection = vector(camera.target.x - transform.position.x,
		camera.target.y - transform.position.y,
		camera.target.z - transform.position.z)
	viewDirection = viewDirection:normalize()

	camera.offset = vector(0, 0, 0)

	velocity.x = velocity.x * 0.8
	velocity.z = velocity.z * 0.8

	if input.IsKeyDown(87) then -- W
		velocity.x = velocity.x + delta * viewDirection.x * (40 + 60 * bton(input.IsKeyDown(340)))
		velocity.z = velocity.z + delta * viewDirection.z * (40 + 60 * bton(input.IsKeyDown(340)))
	end
	if input.IsKeyDown(83) then -- S
		velocity.x = velocity.x - delta * viewDirection.x * (50 + 50 * bton(input.IsKeyDown(340)))
		velocity.z = velocity.z - delta * viewDirection.z * (50 + 50 * bton(input.IsKeyDown(340)))
	end
	if input.IsKeyDown(68) then -- A
		velocity.x = velocity.x - delta * viewDirection.z * math.sin(math.rad(90)) * (50 + 50 * bton(input.IsKeyDown(340)))
		velocity.z = velocity.z - delta * viewDirection.x * (-math.sin(math.rad(90))) * (50 + 50 * bton(input.IsKeyDown(340)))
	end
	if input.IsKeyDown(65) then -- D
		velocity.x = velocity.x + delta * viewDirection.z * math.sin(math.rad(90)) * (50 + 50 * bton(input.IsKeyDown(340)))
		velocity.z = velocity.z + delta * viewDirection.x * (-math.sin(math.rad(90))) * (50 + 50 * bton(input.IsKeyDown(340)))
	end
	if (input.IsKeyDown(32) and grounded) then -- Space
		if input.IsKeyPressed(32) or velocity.y < 0 then
			velocity.y = 0.5
		end

		velocity.y = velocity.y + 60 * delta

		if velocity.y > 6 then
			velocity.y = 6
			grounded = false
		end
	end

	if input.IsKeyReleased(32) then
		grounded = false
	end
	
	print(velocity.y)

	transform.position.x = transform.position.x + velocity.x * delta
	transform.position.y = transform.position.y + velocity.y * delta
	transform.position.z = transform.position.z + velocity.z * delta

	if transform.position.y < 0.5 then
		transform.position.y = 0.5
		grounded = true
	else
		velocity.y = velocity.y - 10 * delta
	end

	camera.target.x = transform.position.x + viewDirection.x
	camera.target.y = transform.position.y + viewDirection.y
	camera.target.z = transform.position.z + viewDirection.z

	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "vector", velocity)
	scene.SetComponent(self.ID, "camera", camera.ID, camera.offset, camera.target)

end

return playerController

