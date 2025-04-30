#pragma once
#include "entt.hpp"
#include "Components.hpp"
#include "GameConsole.hpp"
#include "Scene.hpp"

#include <vector>
#include <algorithm>

#include "raylib.h"  
#include "raymath.h" 


class System
{
	public:
	virtual bool OnUpdate(entt::registry & registry, float delta) = 0;
};

class PoisonSystem : public System
{
	int m_lifetime;

public:
	PoisonSystem(int lifetime) : m_lifetime(lifetime) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{
		auto view = registry.view<c_Health, c_Poison>();
		view.each([](c_Health& health, const c_Poison& poison) {
			health.Value -= poison.TickDamage;
			});

		return (--m_lifetime) <= 0;
	}
};

class SpawnPoisonSystem : public System
{
	int m_lifetime;

public:
	SpawnPoisonSystem(int lifetime) : m_lifetime(lifetime) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{
		auto view = registry.view<c_Health>(entt::exclude<c_Poison>);

		view.each([&](entt::entity entity, const c_Health& health) {
			if (rand() % 4 == 0)
			{
				//Poison an entity
				float tickDamage = float(rand() % 10 + 1) / 10; //Random  [1;10]
				registry.emplace<c_Poison>(entity, tickDamage);
			}
			});

		return (--m_lifetime) <= 0;
	}
};

class CurePoisonSystem : public System
{
	int m_lifetime;

public:
	CurePoisonSystem(int lifetime) : m_lifetime(lifetime) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{
		if (rand() % 20 == 0)
		{
			auto view = registry.view<c_Poison>();

			view.each([&](entt::entity entity, const c_Poison& poison) {
				registry.remove<c_Poison>(entity);
				});
		}

		return (--m_lifetime) <= 0;
	} 
};

class DeathSystem : public System
{
	int m_lifetime;

public:
	DeathSystem(int lifetime) : m_lifetime(lifetime) {}

	bool OnUpdate(entt::registry& registry, float delta) final
	{
		auto view = registry.view<c_Health>();

		view.each([&](entt::entity entity, const c_Health& health) {
			if (health.Value <= 0.0f)
			{
				registry.destroy(entity);
			}
			});

		return (--m_lifetime) <= 0;
	}
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

class SelectionSystem : public System
{
	lua_State* L;
    Vector2 m_initialClickPos;
    bool m_isDragging = false;

public:
	SelectionSystem(lua_State* lua) : L(lua) {}

    bool OnUpdate(entt::registry& registry, float delta) override
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            m_initialClickPos = GetMousePosition();

            // Check if we're clicking on empty space to deselect
            bool clickedOnEntity = false;

            auto view = registry.view<const c_Transform, const c_Collision, const c_Clickable>();
            for (auto [entity, transform, collision] : view.each()) {
                Rectangle rect = {
                    transform.position.x,
                    transform.position.y,
                    collision.width,
                    collision.height
                };

                if (CheckCollisionPointRec(m_initialClickPos, rect)) {
                    clickedOnEntity = true;

                    // For multi-selection with shift
                    if (!IsKeyDown(KEY_LEFT_SHIFT)) { 
                        auto selectedView = registry.view<c_Selected>();
                        registry.remove<c_Selected>(selectedView.begin(), selectedView.end());
                    }

                    // Toggle selection
                    if (registry.all_of<c_Selected>(entity)) {
                        registry.remove<c_Selected>(entity);
                    }
                    else {
                        registry.emplace<c_Selected>(entity);
                    }

                    break;
                }
            }

            // Clicked on empty space - deselect all
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
	lua_State* L;
    Vector2 m_dragOffset;
    bool m_isDragging = false;

public:
	DraggingSystem(lua_State* lua) : L(lua) {}

    bool OnUpdate(entt::registry& registry, float delta) override
    {
        auto view = registry.view<c_Selected, c_Transform>();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !registry.view<c_Selected>().empty()) {
            // Store initial offset for smooth dragging
            Vector2 mousePos = GetMousePosition();
            view.each([&](entt::entity entity, c_Transform& transform) {
                m_dragOffset = {
                    transform.position.x - mousePos.x,
                    transform.position.y - mousePos.y
                };
                m_isDragging = true;
                return; // Just need the first entity for offset
                });
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            m_isDragging = false;
        }

        if (m_isDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();

            view.each([&](entt::entity entity, c_Transform& transform) {
                transform.position.x = mousePos.x + m_dragOffset.x;
                transform.position.y = mousePos.y + m_dragOffset.y;
                });
        }

        return false;
    }
};
//class VelocitySystem : public System
//{
//	lua_State* L;
//
//public:
//	VelocitySystem(lua_State* L) : L(L) {}
//
//	bool OnUpdate(entt::registry& registry, float delta) final
//	{
//		auto view = registry.view<c_Velocity, c_Transform>();
//
//		view.each([&](c_Velocity& velocity, c_Transform& transform)
//			{
//				transform.position.x += velocity.x * delta;
//				transform.position.y += velocity.y * delta;
//				transform.position.z += velocity.z * delta;
//			});
//
//		return false;
//	}
//};