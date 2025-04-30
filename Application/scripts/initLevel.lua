local entity = scene.CreateEntity()

--Get player starting position from level file

scene.SetComponent(entity, "behaviour", "scripts/playerController.lua")