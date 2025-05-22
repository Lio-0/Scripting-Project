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

		posX = 550,
		posY = 100,
		width = 100,
		height = 40,
		textPosX = 10,
		textPosY = 15,
		fontSize = 10,
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


	scene.SetComponent(self.ID, "button", button)
	scene.SetComponent(self.ID, "color", color)
end

function saveAndExitButton:OnClick(delta)
	system.LoadScene("menu")
	system.ResetScene()
end

function saveAndExitButton:OnReset(delta)
end

return saveAndExitButton