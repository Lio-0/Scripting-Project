local filur = {}

function filur:OnCreate()
	print("Filur Created!");
end

function filur:OnUpdate(delta)

	local transform = scene.GetComponent(self.ID, "transform")
	local health = scene.GetComponent(self.ID, "health")

	health = health - 1
	transform.position.x = transform.position.x + delta;

	scene.SetComponent(self.ID, "health", health)
	scene.SetComponent(self.ID, "transform", transform)
end

return filur