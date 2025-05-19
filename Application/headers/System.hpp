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
		if (IsKeyPressed(KEY_ONE))
			activeCamID = 0;
		if (IsKeyPressed(KEY_TWO))
			activeCamID = 1;

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

		if (activeCamID == 0)
		{
			camera->position = { t.position.x + c.positionOffset.x, t.position.y + c.positionOffset.y, t.position.z + c.positionOffset.z };
			camera->target = { c.target.x, c.target.y, c.target.z };

			UpdateCameraPro(camera,
				Vector3{ 0, 0, 0 },
				Vector3{
					GetMouseDelta().x * delta * 2,   // Rotation: yaw
					GetMouseDelta().y * delta * 2,   // Rotation: pitch
					0.0f                         // Rotation: roll
				},
				0);
		}
		else
		{
			if (IsCursorHidden())
			{
				UpdateCamera(camera, CAMERA_FREE);


				if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
					EnableCursor();

				
			}
			if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
				DisableCursor();
		}

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

class SelectionSystem : public System
{
	lua_State* L;
	Vector2 m_initialClickPos;
	bool m_isDragging = false;
	Camera* m_camera;

public:
	SelectionSystem(lua_State* lua, Camera* camera) : L(lua), m_camera(camera) {}

	bool OnUpdate(entt::registry& registry, float delta) override
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			m_initialClickPos = GetMousePosition();
			Ray mouseRay = GetMouseRay(m_initialClickPos, *m_camera);

			bool clickedOnEntity = false;

			auto view = registry.view<const c_Transform, const c_Collision, const c_Clickable>();
			for (auto [entity, transform, collision] : view.each()) {
				// Create bounding box from transform and collision components
				BoundingBox box;
				box.min = {
					transform.position.x - transform.scale.x / 2, // Using transform.scale instead of collision.width
					transform.position.y - transform.scale.y / 2,
					transform.position.z - transform.scale.z / 2
				};
				box.max = {
					transform.position.x + transform.scale.x / 2,
					transform.position.y + transform.scale.y / 2,
					transform.position.z + transform.scale.z / 2
				};

				RayCollision collisionInfo = GetRayCollisionBox(mouseRay, box);
			if (collisionInfo.hit) {
					clickedOnEntity = true;

					if (!IsKeyDown(KEY_LEFT_SHIFT)) {
						auto selectedView = registry.view<c_Selected>();
						registry.remove<c_Selected>(selectedView.begin(), selectedView.end());
						if (registry.all_of<c_Color>(entity)) {
							auto& color = registry.get<c_Color>(entity);
							color.r = 255;
							color.g = 255;
							color.b = 255;
						}
					}
					if (registry.all_of<c_Selected>(entity)) {
						registry.remove<c_Selected>(entity);
						if (registry.all_of<c_Color>(entity)) {
							auto& color = registry.get<c_Color>(entity);
							color.r = 255;
							color.g = 255;
							color.b = 255;
						}
					}
					else {
						registry.emplace<c_Selected>(entity);
						if (registry.all_of<c_Color>(entity)) {
							auto& color = registry.get<c_Color>(entity);
							color.r = 0;
							color.g = 255;
							color.b = 0;
						}
					}

					break;
				}
			}



			if (!clickedOnEntity && !IsKeyDown(KEY_LEFT_SHIFT)) {
				auto selectedView = registry.view<c_Selected>();
				registry.remove<c_Selected>(selectedView.begin(), selectedView.end());
			}
		}

		return false;
	}
};

class DraggingSystem : public System
{
	Camera* m_camera;
	entt::entity m_draggedEntity = entt::null;

public:
	DraggingSystem(lua_State*, Camera* camera) : m_camera(camera) {}

	bool OnUpdate(entt::registry& registry, float delta) override
	{
		auto view = registry.view<c_Selected, c_Transform, c_Collision>();

		// Start dragging if mouse pressed on a selected entity
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mousePos = GetMousePosition();
			Ray ray = GetMouseRay(mousePos, *m_camera);

			float closestDistance = FLT_MAX;
			entt::entity hitEntity = entt::null;

			for (auto [entity, transform, collision] : view.each()) {
				BoundingBox box = {
					box.min.x = transform.position.x - transform.scale.x * 0.5f,
					box.min.y = transform.position.y - transform.scale.y * 0.5f,
					box.min.z = transform.position.z - transform.scale.z * 0.5f,

					box.max.x = transform.position.x + transform.scale.x * 0.5f,
					box.max.y = transform.position.y + transform.scale.y * 0.5f,
					box.max.z = transform.position.z + transform.scale.z * 0.5f
				};
				RayCollision hit = GetRayCollisionBox(ray, box);
				if (hit.hit && hit.distance < closestDistance) {
					closestDistance = hit.distance;
					hitEntity = entity;
				}
			}

			if (hitEntity != entt::null) {
				m_draggedEntity = hitEntity;
			}
		}

		// Stop dragging on release
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			if (m_draggedEntity != entt::null && registry.all_of<c_Color>(m_draggedEntity)) {
				auto& color = registry.get<c_Color>(m_draggedEntity);
				color.r = 255;
				color.g = 255;
				color.b = 255;
			}
			m_draggedEntity = entt::null;
		}

		// Drag if holding mouse button and an entity is active
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && m_draggedEntity != entt::null) {
			auto& transform = registry.get<c_Transform>(m_draggedEntity);

			Vector2 mousePos = GetMousePosition();
			Ray ray = GetMouseRay(mousePos, *m_camera);

			// Calculate a drag distance based on initial position
			float dragDistance = Vector3Distance(m_camera->position, { transform.position.x, transform.position.y, transform.position.z });

			// Project along the ray at the same approximate distance
			Vector3 target = Vector3Add(ray.position, Vector3Scale(ray.direction, dragDistance));

			transform.position.x = target.x;
			transform.position.y = target.y;
			transform.position.z = target.z;

			if (registry.all_of<c_Color>(m_draggedEntity)) {
				auto& color = registry.get<c_Color>(m_draggedEntity);
				color.r = 0;
				color.g = 200;
				color.b = 0;
			}
			return false;
		}
	};
};

class ButtonSystem : public System
{
	lua_State* L;
public:
	ButtonSystem(lua_State* L) : L(L) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{
		auto view = registry.view<c_Button, c_Behaviour>();

		view.each([&](c_Button button, c_Behaviour script)
			{
				if (CheckCollisionPointRec(GetMousePosition(), Rectangle(button.posX, button.posY, button.width, button.height)))
				{
					lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaTableRef);
					lua_getfield(L, -1, "OnClick");
					lua_pushvalue(L, -2);
					lua_pushnumber(L, delta);

					if (lua_pcall(L, 2, 0, 0) != LUA_OK)
					{
						GameConsole::DumpError(L);
					}

					lua_pop(L, 1);
				}
			}
		);
	}
};