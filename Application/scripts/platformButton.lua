local platformButton = {}
local mainColor = {a = 255, r = 255, g = 255, b = 255}
local button = {}
local co = coroutine.create(function() end)

function platformButton:OnCreate()
	button = {
		textColour = {
		a = 255,
		r = 0,
		g = 0,
		b = 0
		},

		posX = 100,
		posY = 100,
		width = 100,
		height = 40,
		textPosX = 10,
		textPosY = 15,
		fontSize = 10,
		label = 'Create Platform'
	}
	
	scene.SetComponent(self.ID, "color", mainColor)
	scene.SetComponent(self.ID, "button", button)
end

function platformButton:OnUpdate(delta)
	local color = scene.GetComponent(self.ID, "color")

	local mouseX, mouseY = input.GetMousePosition()

	if (button.posX < mouseX) and  (mouseX < button.posX + button.width) and (button.posY < mouseY) and (mouseY < button.posY + button.height) then
		color = {
			r = mainColor.r - 50,
			g = mainColor.g - 50,
			b = mainColor.b - 50
		}
	else
		color = mainColor
	end

	scene.SetComponent(self.ID, "color", color)
	scene.SetComponent(self.ID, "button", button)

	if (coroutine.status(co) == "suspended") and (input.IsKeyPressed(80) or input.IsKeyPressed(79)) then --Space
		coroutine.resume(co, input.IsKeyPressed(79))
	end
end

function platformButton:OnClick(delta)
	co = coroutine.create( function ()
		system.ToggleBlockChoice()
		local entity = scene.CreateEntity()

		local transform = {
			position = {
				x = 0,
				y = 0,
				z = 0
			},

			rotation = {
				x = 0,
				y = 0,
				z = 0
			},

			scale = {
				x = 4,
				y = 0.5,
				z = 4
			}
		}

		scene.SetComponent(entity, "visual", "cube", "rock_texture", true) 
		scene.SetComponent(entity, "collision", 1)
		scene.SetComponent(entity, "transform", transform)
		scene.SetComponent(entity, "clickable")
		scene.SetComponent(entity, "color", {r = 255, g = 255, b = 255})
		scene.SetComponent(entity, "vector", transform.rotation)

		if (coroutine.yield() and scene.IsEntity(entity)) then
			scene.SetComponent(entity, "behaviour", "scripts/obstacle.lua")
		end
		system.ToggleBlockChoice()
	end)
	coroutine.resume(co)
end

function platformButton:OnReset(delta)
end

return platformButton