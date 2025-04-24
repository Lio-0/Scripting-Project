#pragma once
#include "entt.hpp"

class System
{
	public:
	virtual bool OnUpdate(entt::registry & registry, float delta) = 0;
};
