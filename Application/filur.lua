local filur = {}

function filur:OnCreate()
	print("Filur Created!");
end

function filur:OnUpdate(delta)
	print("Filur Updated!")

	--[[self.vector = self.vector + vector(0.1, 0, 0.1)

	self.health = self.health - 1;

	if self.health <= 0 then
		scene.RemoveEntity(self.ID)
	end]]--
end

return filur