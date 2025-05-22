local obstacle = {}
local timer = 0

function obstacle:OnCreate()

	scene.SetComponent(self.ID, "collision", 2)
	scene.SetComponent(self.ID, "color", {r = 255, g = 0, b = 0})
	scene.SetComponent(self.ID, "vector", {x = 0, y = 0, z = 0})
end

function obstacle:OnUpdate(delta)

	if (system.PlayerState() == 1) then
		timer = timer + delta

		if (timer > 1.0) then
			system.ResetScene()
		end
	else 
		timer = 0.0
	end
end

function obstacle:OnCollision(delta, collisionX, collisionY, collisionZ)

	if ((collisionX or collisionY or collisionZ) and (system.PlayerState() == 0)) then
        system.Lose()
		timer = 0.0
	end
end

function obstacle:OnReset(delta)
end

return obstacle