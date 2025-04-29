#pragma once
#include "entt.hpp"
#include "Components.hpp"
#include "GameConsole.hpp"
#include "Scene.hpp"


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