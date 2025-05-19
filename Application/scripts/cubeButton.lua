local cubeButton = {}

local vector = require("vector")
local mainColor

function cubeButton:OnCreate()

	mainColor = {a = 150, r = 255, g = 255, b = 255}
	scene.SetComponent(self.ID, "color", mainColor)

	button = {
		textColour = {
		a = 255,
		r = 0,
		g = 0,
		b = 0
		},

		posX = 100,
		posY = 100,
		textPosX = 20,
		textPosY = 20
		fontSize = 50,
		width = 100,
		height = 60,
		label = "Create Cube"
	}
	
	scene.SetComponent(entity, "color", black)

end

function cubeButton:OnUpdate(delta)
	local color = scene.GetComponent(self.ID, "color")
	local button = scene.GetComponent(self.ID, "button")

	mouseX, mouseY = input.GetMousePosition()

	if button.posX < mouseX < button.posX + button.width and button.posY < mouseY < button.posY + button.height then
		color = {
			a = 150,
			r = mainColor.r - 50,
			g = mainColor.g - 50,
			b = mainColor.b - 50
		}
	else
		color = mainColor
	end

	scene.SetComponent(self.ID, "color", color)
end

function cubeButton:OnClick(delta)
	dofile("createBlock.lua")
end

return cubeButton