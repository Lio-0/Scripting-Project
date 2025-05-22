local orb = {}

local vector = require("vector")

function orb:OnCreate()
end

function orb:OnUpdate(delta)
	local transform = scene.GetComponent(self.ID, "transform")

	transform.rotation.x = transform.rotation.x + delta * 100

	scene.SetComponent(self.ID, "transform", transform)
end

function orb:OnCollision(delta, collisionX, collisionY, collisionZ)
	if (collisionX or collisionY or collisionZ) then
		scene.RemoveEntity(self.ID)
	end
end

return orb