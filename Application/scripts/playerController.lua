local playerController = {}
local vector = require("vector")
local grounded
local colX, colY, colZ = false, false, false
local lastPos
local timer = 0

function wait(t)
 	local start = os.time()
 	repeat until os.time() > start + t
end

local function bton(value)
	return value == true and 1 or value == false and 0
end

function playerController:OnCreate()
	local transform = {
		position = {
			x = 0,
			y = 2,
			z = 0
		},

		rotation = {
			x = 0,
			y = 0,
			z = 0
		},

		scale = {
			x = 0.5,
			y = 2,
			z = 0.5
		}
	}

	local vector = {
		x = 0,
		y = 0,
		z = 0
	}

	lastPos = transform.position

	scene.SetComponent(self.ID, "vector", vector)

	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "collision", 0)

	--Transform is reused for vectors required in the camera component, position is offset, rotation is target
	transform.position.y = 0.9 --Camera height
	transform.rotation.z = 1 --Camera look direction
	scene.SetComponent(self.ID, "camera", 0, transform.position, transform.rotation)
end

function playerController:OnUpdate(delta)
	--Get data from components
	local transform = scene.GetComponent(self.ID, "transform")
	local velocity = scene.GetComponent(self.ID, "vector")
	local camera = scene.GetComponent(self.ID, "camera")

	lastPos.x = transform.position.x
	lastPos.y = transform.position.y
	lastPos.z = transform.position.z

	-- Get the view direction of the camera
	local viewDirection = vector(camera.target.x - transform.position.x - camera.offset.x,
		camera.target.y - transform.position.y - camera.offset.y,
		camera.target.z - transform.position.z - camera.offset.z):normalize()

	-- Get a version without y component for movement
	viewDirNoY = vector(viewDirection.x, 0, viewDirection.z):normalize() 

	velocity.x = velocity.x * 0.8
	velocity.z = velocity.z * 0.8

	--Controls---------------------------------------------------------------------------------------------------------------

	if (system.PlayerState() == 0) then
		-- W = 87
		if input.IsKeyDown(87) then 
			velocity.x = velocity.x + delta * viewDirNoY.x * (30 + 50 * bton(input.IsKeyDown(340)))
			velocity.z = velocity.z + delta * viewDirNoY.z * (30 + 50 * bton(input.IsKeyDown(340)))
		end

		-- S = 83
		if input.IsKeyDown(83) then 
			velocity.x = velocity.x - delta * viewDirNoY.x * (30 + 50 * bton(input.IsKeyDown(340)))
			velocity.z = velocity.z - delta * viewDirNoY.z * (30 + 50 * bton(input.IsKeyDown(340)))
		end

		-- A = 68
		if input.IsKeyDown(68) then 
			velocity.x = velocity.x - delta * viewDirNoY.z * math.sin(math.rad(90)) * (30 + 50 * bton(input.IsKeyDown(340)))
			velocity.z = velocity.z - delta * viewDirNoY.x * (-math.sin(math.rad(90))) * (30 + 50 * bton(input.IsKeyDown(340)))
		end

		-- D = 65
		if input.IsKeyDown(65) then
			velocity.x = velocity.x + delta * viewDirNoY.z * math.sin(math.rad(90)) * (30 + 50 * bton(input.IsKeyDown(340)))
			velocity.z = velocity.z + delta * viewDirNoY.x * (-math.sin(math.rad(90))) * (30 + 50 * bton(input.IsKeyDown(340)))
		end

		-- Space = 32
		if (input.IsKeyDown(32) and grounded) then

			--Initial launch speed for better gameplay feel
			if input.IsKeyPressed(32) or velocity.y < 0 then
				velocity.y = 0.3
			end

			--Accelerate upwards as long as player is holding space
			velocity.y = velocity.y + 60 * delta

			--Stop when upwards velocity limit is reached
			if velocity.y > 6 then
				velocity.y = 6
				grounded = false
			end
		end
	end

	--End jump acceleration if player stops pressing space
	if input.IsKeyReleased(32) then
		grounded = false
	end
	------------------------------------------------------------------------------------------------------------------------


	--Update player position
	transform.position.x = transform.position.x + velocity.x * delta
	transform.position.y = transform.position.y + velocity.y * delta
	transform.position.z = transform.position.z + velocity.z * delta
	

	if (velocity.y < 0) then
		grounded = false
	end

	if (colX) then
		transform.position.x = lastPos.x
		colX = false
	end
	if (colY) then
		transform.position.y = lastPos.y
		if velocity.y <= 0 then
			grounded = true
			velocity.y = 0
		end
		colY = false
	end
	if (colZ) then
		transform.position.z = lastPos.z
		colZ = false
	end

	if (system.PlayerState() ~= 2) then
		velocity.y = velocity.y - 10 * delta
	else
		velocity.y = 0
		velocity.x = velocity.x + 10
		velocity.z = velocity.z + 10
	end

	if velocity.y < -10 then
		velocity.y = -10
	end

	--Update camera target location based on viewDirection and player position
	camera.target.x = transform.position.x + viewDirection.x + camera.offset.x
	camera.target.y = transform.position.y + viewDirection.y + camera.offset.y
	camera.target.z = transform.position.z + viewDirection.z + camera.offset.z
	

	--Save data to components
	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "vector", velocity)
	scene.SetComponent(self.ID, "camera", camera.ID, camera.offset, camera.target)

	--Current collision calculation
	if (transform.position.y < -10) and (system.PlayerState() == 0) then
		timer = 0.0
		system.Lose()
	end

	if system.PlayerState() == 1 then
		timer = timer + delta

		if (timer > 1.0) then
			system.ResetScene()
		end
	else 
		timer = 0.0
	end
end


function playerController:OnCollision(delta, collisionX, collisionY, collisionZ)
	local t = scene.GetComponent(self.ID, "transform")
	local v = scene.GetComponent(self.ID, "vector")

	colX = collisionX
	colY = collisionY
	colZ = collisionZ

	if (collisionY) then
		if v.y > 0 then
			v.y = -v.y
		end
	end

	scene.SetComponent(self.ID, "transform", t)
	scene.SetComponent(self.ID, "vector", v)
end

function playerController:OnReset(delta)

	local transform = scene.GetComponent(self.ID, "transform")
	local velocity = scene.GetComponent(self.ID, "vector")

	local transform = {
		position = {
			x = 0,
			y = 2,
			z = 0
		},

		rotation = {
			x = 0,
			y = 0,
			z = 0
		},

		scale = {
			x = 0.3,
			y = 2,
			z = 0.3
		}
	}

	local vector = {
		x = 0,
		y = 0,
		z = 0
	}

	scene.SetComponent(self.ID, "vector", vector)
	scene.SetComponent(self.ID, "transform", transform)
end

return playerController

