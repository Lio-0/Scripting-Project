local filur = {}

function filur:OnCreate()
end

function filur:OnUpdate(delta)

	local transform = scene.GetComponent(self.ID, "transform")
	local health = scene.GetComponent(self.ID, "health")

	health = health - 1
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

	scene.SetComponent(self.ID, "health", health)
	scene.SetComponent(self.ID, "transform", transform)
end

return filur