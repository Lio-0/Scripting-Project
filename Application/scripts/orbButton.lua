local orbButton = {}
local mainColor = {a = 255, r = 255, g = 255, b = 255}
local button = {}

function orbButton:OnCreate()
	button = {
		textColour = {
		a = 255,
		r = 0,
		g = 0,
		b = 0
		},

		posX = 250,
		posY = 100,
		width = 100,
		height = 40,
		textPosX = 20,
		textPosY = 15,
		fontSize = 10,
		label = 'Create Orb'
	}
	
	scene.SetComponent(self.ID, "color", mainColor)
	scene.SetComponent(self.ID, "button", button)
end

function orbButton:OnUpdate(delta)
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
end

function orbButton:OnClick(delta)
	co = coroutine.create(function ()
           local entity = scene.CreateEntity()
			scene.SetComponent(entity, "behaviour", "scripts/orb.lua")
         end)
	coroutine.resume(co)
end

return orbButton