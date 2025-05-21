local playButton = {}
local mainColor = {a = 255, r = 255, g = 255, b = 255}
local button = {}

function playButton:OnCreate()
	button = {
		textColour = {
		a = 255,
		r = 0,
		g = 0,
		b = 0
		},

		posX = 0,
		posY = 0,
		width = 180,
		height = 60,
		textPosX = 20,
		textPosY = 10,
		fontSize = 40,
		label = 'Play'
	}
	
	scene.SetComponent(self.ID, "color", mainColor)
	scene.SetComponent(self.ID, "button", button)
end

function playButton:OnUpdate(delta)
	local color = scene.GetComponent(self.ID, "color")

	local mouseX, mouseY = input.GetMousePosition()
	local screenX, screenY = system.GetScreenDim()

	if (button.posX < mouseX) and (mouseX < button.posX + button.width) and (button.posY < mouseY) and (mouseY < button.posY + button.height) then
		color = {
			r = mainColor.r - 50,
			g = mainColor.g - 50,
			b = mainColor.b - 50
		}
	else
		color = mainColor
	end

	button.posX = screenX / 2 - 90
	button.posY = screenY / 2 + 50

	scene.SetComponent(self.ID, "button", button)
	scene.SetComponent(self.ID, "color", color)
end

function playButton:OnClick(delta)
	input.DisableCursor()
	system.LoadScene("game")
	system.ResetScene()
end

return playButton