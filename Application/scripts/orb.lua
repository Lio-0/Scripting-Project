local orb = {}

local vector = require("vector")

function orb:OnCreate()
	local transform = {
		position = {
			x = math.random(0, 10),
			y = 1,
			z = math.random(0, 10)
		},

		rotation = {
			x = 0,
			y = 0,
			z = 0
		},

		scale = {
			x = 0.3,
			y = 0.3,
			z = 0.3
		}
	}
	scene.SetComponent(self.ID, "visual", "orb", "orb_texture", true) 
	scene.SetComponent(self.ID, "collision", 2)
	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "vector", transform.rotation)
	scene.SetComponent(self.ID, "collectible", 0)
	scene.SetComponent(self.ID, "clickable")
	scene.SetComponent(self.ID, "color", {r = 255, g = 255, b = 255})


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