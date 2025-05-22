local obstacle = {}

function wait(t)
 	local start = os.time()
 	repeat until os.time() > start + t
end

function obstacle:OnCreate()

	scene.SetComponent(self.ID, "collision", 2)
	scene.SetComponent(self.ID, "color", {r = 255, g = 0, b = 0})
	scene.SetComponent(self.ID, "vector", {x = 0, y = 0, z = 0})
end

function obstacle:OnUpdate(delta)
end

function obstacle:OnCollision(delta, collisionX, collisionY, collisionZ)

	if (collisionX or collisionY or collisionZ) then
		wait(1)
		system.ResetScene()
	end
end

function obstacle:OnReset(delta)
end

return obstacle