local filur = {}

function filur:OnCreate()
	local transform = {
		position = {
			x = math.random(-10, 10),
			y = 0,
			z = math.random(-10, 10)
		},

		rotation = {
			x = 0,
			y = 0,
			z = 0
		},

		scale = {
			x = 1.0,
			y = 1.0,
			z = 1.0
		}
	}

	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "visual", "cube", "", true) 
end

function filur:OnUpdate(delta)
	local transform = scene.GetComponent(self.ID, "transform")

	transform.position.x = transform.position.x + delta

	-- 32 = Space
	if input.IsKeyDown(32) then
		transform.position.y = transform.position.y + delta
	else
		transform.position.y = transform.position.y - delta / transform.position.x
		if transform.position.y < 0 then
			transform.position.y = 0
		end
	end

	transform.rotation.x = transform.rotation.x + delta * 100;
	
	scene.SetComponent(self.ID, "transform", transform)
end

return filur