local saveAndExitButton = {}
local mainColor = {a = 255, r = 255, g = 255, b = 255}
local button = {}

function saveAndExitButton:OnCreate()
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
		textPosX = 40,
		textPosY = 10,
		fontSize = 40,
		label = 'Save & Exit'
	}
	
	scene.SetComponent(self.ID, "color", mainColor)
	scene.SetComponent(self.ID, "button", button)
end

function saveAndExitButton:OnUpdate(delta)
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

	button.posX = screenX / 2 - 80
	button.posY = screenY / 2 + 60

	scene.SetComponent(self.ID, "button", button)
	scene.SetComponent(self.ID, "color", color)
end

function saveAndExitButton:OnClick(delta)
	system.LoadScene("menu")
	system.ResetScene()
end

return saveAndExitButton