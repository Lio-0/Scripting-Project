local platformButton = {}
local mainColor = {a = 255, r = 255, g = 255, b = 255}

function platformButton:OnCreate()


	local button = {
		textColour = {
		a = 255,
		r = 0,
		g = 0,
		b = 0
		},

		posX = 200,
		posY = 200,
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
	local button = scene.GetComponent(self.ID, "button")

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
end

function platformButton:OnClick(delta)
	dofile("scripts/createPlatform.lua")
end

return platformButton