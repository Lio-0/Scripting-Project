local goal = {}
local locked = true
local timer

function goal:OnCreate()
    locked = true
end

function goal:OnUpdate(delta)
	local open = scene.GetComponent(self.ID, "goal")
    if locked and open then
		locked = false
        scene.SetComponent(self.ID, "visual", "goal_open", "goal_open", true)
    end

    if (system.PlayerState() == 2) then
        timer = timer + delta

        if (timer > 1.0) then
		    system.LoadScene("menu")
		    input.EnableCursor()
        end
    else
        timer = 0.0
    end
end

function goal:OnCollision(delta, collisionX, collisionY, collisionZ)
	if ((collisionX or collisionY or collisionZ) and not locked) then
        system.Win()
        timer = 0.0
    end
end

function goal:OnReset(delta)
    scene.SetComponent(self.ID, "goal", false)
    scene.SetComponent(self.ID, "visual", "goal_locked", "goal_locked", true) 
    locked = true
end

return goal