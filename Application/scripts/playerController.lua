local playerController = {}

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

	vector.y = 0.5;

	scene.SetComponent(self.ID, "camera", 0, vector)
	scene.SetComponent(self.ID, "transform", transform)
end

function playerController:OnUpdate(delta)
	local transform = scene.GetComponent(self.ID, "transform")
	local velocity = scene.GetComponent(self.ID, "vector")
	local camera = scene.GetComponent(self.ID, "camera")

	if input.IsKeyDown(87) then -- W
		velocity.x = velocity.x + delta * 100
	end
	if input.IsKeyDown(83) then -- S
		velocity.x = velocity.x - delta * 100
	end
	if input.IsKeyDown(68) then -- A
		velocity.z = velocity.z + delta * 100
	end
	if input.IsKeyDown(65) then -- D
		velocity.z = velocity.z - delta * 100
	end
	if input.IsKeyPressed(32) then -- Space
		velocity.y = 10
	end

	if transform.position.y < 0.5 then
		transform.position.y = 0.5
	else
		velocity.y = velocity.y - 0.5
	end

	velocity.x = velocity.x * 0.9
	velocity.z = velocity.z * 0.9

	transform.position.x = transform.position.x + velocity.x * delta
	camera.target.x = transform.position.x + velocity.x * delta
	transform.position.y = transform.position.y + velocity.y * delta
	camera.target.y = transform.position.y + velocity.y * delta
	transform.position.z = transform.position.z + velocity.z * delta 
	camera.target.z = transform.position.z + velocity.z * delta

	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "vector", velocity)
	scene.SetComponent(self.ID, "camera", camera)
end

return playerController

