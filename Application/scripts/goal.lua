local goal = {}
local locked = true

function wait(t)
 	local start = os.time()
 	repeat until os.time() > start + t
end

function goal:OnCreate()
	local transform = {
		position = {
			x = 2,
			y = 1,
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

	scene.SetComponent(self.ID, "goal", false)
	scene.SetComponent(self.ID, "visual", "goal_locked", "goal_locked", true) 
	scene.SetComponent(self.ID, "collision", 2)
	scene.SetComponent(self.ID, "transform", transform)
	scene.SetComponent(self.ID, "vector", transform.rotation)
	scene.SetComponent(self.ID, "color", {r = 255, g = 255, b = 255})
	scene.SetComponent(self.ID, "clickable")
end

function goal:OnUpdate(delta)
	local open = scene.GetComponent(self.ID, "goal")
    if locked and open then
		locked = false
        scene.SetComponent(self.ID, "visual", "goal_open", "goal_open", true)
    end
end

function goal:OnCollision(delta, collisionX, collisionY, collisionZ)

	if ((collisionX or collisionY or collisionZ) and not locked) then
        co = coroutine.create(function ()
			wait(2)
			system.LoadScene("menu")
			input.EnableCursor()
            end)
	    coroutine.resume(co)
    end
end

return goal