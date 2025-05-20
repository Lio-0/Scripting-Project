local goal = {}
local locked = true

function wait(t)
 	local start = os.time()
 	repeat until os.time() > start + t
end

function goal:OnCreate()
	local transform = {
		position = {
			x = 0,
			y = 8,
			z = 0
		},

		rotation = {
			x = 0,
			y = 0,
			z = 0
		},

		scale = {
			x = 3,
			y = 3,
			z = 3
		}
	}

	scene.SetComponent(self.ID, "visual", "goal_locked", "goal_locked", true) 
	scene.SetComponent(self.ID, "collision", 2)
	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "vector", transform.rotation)
	scene.SetComponent(self.ID, "clickable")
	scene.SetComponent(self.ID, "color", {r = 255, g = 255, b = 255})
end

function goal:OnUpdate(delta)

    if locked then
		locked = false
        scene.SetComponent(self.ID, "visual", "goal_open", "goal_open", true) )
    end

end

function goal:OnCollisionfunction(delta, collisionX, collisionY, collisionZ)

    if (collisionX or collisionY or collisionZ) then
        co = coroutine.create(function ()
		    wait(10)
            end)
	    coroutine.resume(co)
    end
end

return goal