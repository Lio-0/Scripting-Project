
function orb:OnCreate()
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
			x = 0.25,
			y = 0.25,
			z = 0.25
		}
	}

	lastPos = transform.position

	scene.SetComponent(entity, "transform", transform)
	scene.SetComponent(entity, "collision", 0)

end

function orb:OnUpdate(delta)
	scene.GetComponent()
end

function orb:OnCollision(delta, collisionX, collisionY, collisionZ)
end