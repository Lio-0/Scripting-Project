local goal = {}
local locked = true

function wait(t)
 	local start = os.time()
 	repeat until os.time() > start + t
end

function goal:OnCreate()
end

function goal:OnUpdate(delta)
	local open = scene.GetComponent(self.ID, "goal")
    if locked and open then
		locked = false
        scene.SetComponent(self.ID, "visual", "goal_open", "goal_open", true)
    end
end

function goal:OnCollision(delta, collisionX, collisionY, collisionZ)
    print("Hello")
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