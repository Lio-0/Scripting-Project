#pragma once
#include "entt.hpp"
#include "Components.hpp"
#include "GameConsole.hpp"
#include "Scene.hpp"
#include "raylib.h"
#include "raymath.h"
#include <iostream>

class System
{
	public:
	virtual bool OnUpdate(entt::registry & registry, float delta) = 0;
};

class BehaviourSystem : public System
{
	lua_State* L;

public:
	BehaviourSystem(lua_State* L) : L(L) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{
		auto view = registry.view<c_Behaviour>();

		view.each([&](c_Behaviour& script) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
			lua_getfield(L, -1, "OnUpdate");
			lua_pushvalue(L, -2);
			lua_pushnumber(L, delta);

			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
			{
				GameConsole::DumpError(L);
			}

			lua_pop(L, 1);
			});

		return false;
	}
};

class CameraSystem : public System
{
	Camera* camera;
	int activeCamID;
public:
	CameraSystem(Camera* camera) : camera(camera), activeCamID(0) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{

		auto view = registry.view<c_Camera, c_Transform>();

		c_Camera c;
		c_Transform t;
		view.each([&](c_Camera& cam, c_Transform& transform) {
			if (cam.ID == activeCamID)
			{
				c = cam;
				t = transform;
			}
			});

		camera->position = { t.position.x + c.positionOffset.x, t.position.y + c.positionOffset.y, t.position.z + c.positionOffset.z };
		camera->target = { c.target.x, c.target.y, c.target.z };

		UpdateCameraPro(camera,
			Vector3{0, 0, 0},
			Vector3{
				GetMouseDelta().x * delta * 2,   // Rotation: yaw
				GetMouseDelta().y * delta * 2,   // Rotation: pitch
				0.0f                         // Rotation: roll
			},
			0);                              // Move to target (zoom)

		view.each([&](c_Camera& cam, c_Transform& transform)
			{
				if (cam.ID == activeCamID)
				{
					cam.target = { camera->target.x, camera->target.y, camera->target.z };
				}
			});

		return false;
	}
};

class CollisionSystem : public System
{
	lua_State* L;
	int activeLayer;
	int passiveLayer;

public:
	CollisionSystem(lua_State* L, int active, int passive) : L(L), activeLayer(active), passiveLayer(passive) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{

		auto active = registry.view<c_Collision, c_Transform, c_Vector, c_Behaviour>();
		auto passive = registry.view<c_Collision, c_Transform>();

		active.each([&](c_Collision& aCollision, c_Transform& aT, c_Vector& aVector, c_Behaviour& script)
			{
				if (aCollision.layer == activeLayer)
				{
					bool collisionX = false;
					bool collisionY = false;
					bool collisionZ = false;

					BoundingBox bx;
					bx.min = { aT.position.x - aT.scale.x / 2, aT.position.y - aT.scale.y / 2, aT.position.z - aT.scale.z / 2 };
					bx.max = { aT.position.x + aT.scale.x / 2, aT.position.y + aT.scale.y / 2, aT.position.z + aT.scale.z / 2 };
					BoundingBox by = bx;
					BoundingBox bz = bx;

					bx.min.x += aVector.x * delta;
					bx.max.x += aVector.x * delta;

					by.min.y += aVector.y * delta;
					by.max.y += aVector.y * delta;

					bz.min.z += aVector.z * delta;
					bz.max.z += aVector.z * delta;

					BoundingBox b2;

					passive.each([&](c_Collision& pCollision, c_Transform& pT)
						{
							if (pCollision.layer == passiveLayer)
							{
								b2.min = { pT.position.x - pT.scale.x / 2, pT.position.y - pT.scale.y / 2, pT.position.z - pT.scale.z / 2 };
								b2.max = { pT.position.x + pT.scale.x / 2, pT.position.y + pT.scale.y / 2, pT.position.z + pT.scale.z / 2 };
						
								if (CheckCollisionBoxes(bx, b2))
								{
									collisionX = true;
								}
								if (CheckCollisionBoxes(by, b2))
								{
									collisionY = true;
								}
								if (CheckCollisionBoxes(bz, b2))
								{
									collisionZ = true;
								}
							}
						});


					lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
					lua_getfield(L, -1, "OnCollision");
					lua_pushvalue(L, -2);
					lua_pushnumber(L, delta);
					lua_pushboolean(L, collisionX);
					lua_pushboolean(L, collisionY);
					lua_pushboolean(L, collisionZ);


					if (lua_pcall(L, 5, 0, 0) != LUA_OK)
					{
						GameConsole::DumpError(L);
					}

					lua_pop(L, 1);
				}
			});

		return false;
	}
};