#pragma once
#include "entt.hpp"
#include "Components.hpp"

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
		auto view = registry.view<Health, Poison>();
		view.each([](Health& health, const Poison& poison) {
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
		auto view = registry.view<Health>(entt::exclude<Poison>);

		view.each([&](entt::entity entity, const Health& health) {
			if (rand() % 4 == 0)
			{
				//Poison an entity
				float tickDamage = float(rand() % 10 + 1) / 10; //Random  [1;10]
				registry.emplace<Poison>(entity, tickDamage);
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
			auto view = registry.view<Poison>();

			view.each([&](entt::entity entity, const Poison& poison) {
				registry.remove<Poison>(entity);
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
		auto view = registry.view<Health>();

		view.each([&](entt::entity entity, const Health& health) {
			if (health.Value <= 0.0f)
			{
				registry.destroy(entity);
			}
			});

		return (--m_lifetime) <= 0;
	}
};