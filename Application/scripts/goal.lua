local goal = {}
local locked = true

function wait(t)
 	local start = os.time()
 	repeat until os.time() > start + t
end

function goal:OnCreate()
    locked = true
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
			wait(1)
			system.LoadScene("menu")
			input.EnableCursor()
            end)
	    coroutine.resume(co)
    end
end

function goal:OnReset(delta)
    scene.SetComponent(self.ID, "goal", false)
    scene.SetComponent(self.ID, "visual", "goal_locked", "goal_locked", true) 
    locked = true
end

return goal