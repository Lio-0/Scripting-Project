local entity = scene.CreateEntity()
scene.SetComponent(entity, "vector", math.random(-10, 10), 0, math.random(-10, 10))
scene.SetComponent(entity, "behaviour", "filur.lua")
