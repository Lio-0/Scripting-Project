#pragma once

#include "Scene.hpp"
#include "Components.hpp"

Scene::Scene(lua_State* L)
{
	this->m_luaState = L;
}

int Scene::GetEntityCount()
{
	return m_registry.view<entt::entity>().size();
}

int Scene::CreateEntity()
{
	return (int)m_registry.create();
}

bool Scene::IsEntity(int entity)
{
	return m_registry.valid((entt::entity)entity);
}

void Scene::RemoveEntity(int entity)
{
	m_registry.destroy((entt::entity)entity);
}

void Scene::UpdateSystems(float delta)
{
	for (auto it = m_systems.begin(); it != m_systems.end();)
	{
		if ((*it)->OnUpdate(m_registry, delta))
		{
			delete (*it);
			it = m_systems.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void Scene::lua_openscene(lua_State* L, Scene* scene)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		{"CreateEntity", lua_CreateEntity},
		{"RemoveEntity", lua_RemoveEntity},
		{"IsEntity", lua_IsEntity},
		{"SetComponent", lua_SetComponent},
		{"RemoveComponent", lua_RemoveComponent},
		{NULL, NULL}
	};

	lua_pushlightuserdata(L, scene);
	luaL_setfuncs(L, methods, 1);
	lua_setglobal(L, "scene");
}

Scene* lua_GetSceneUpValue(lua_State* L)
{
	return static_cast<Scene*>(lua_touserdata(L, lua_upvalueindex(1)));
}

void lua_pushtransform(lua_State* L, const c_Transform& transform)
{
	lua_newtable(L);

	
	lua_settable(L, -3);
}

int Scene::lua_CreateEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = scene->CreateEntity();
	lua_pushinteger(L, entity);
	return 1;
}

int Scene::lua_SetComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);

	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);

	if (type == "health")
	{
		float value = lua_tonumber(L, 3);
		scene->SetComponent<c_Health>(entity, value);
	}
	else if (type == "poison")
	{
		float tickDamage = lua_tonumber(L, 3);
		scene->SetComponent<c_Poison>(entity, tickDamage);
	}
	else if (type == "behaviour")
	{
		if (scene->HasComponents<c_Behaviour>(entity))
		{
			scene->RemoveComponent<c_Behaviour>(entity);
		}

		const char* path = lua_tostring(L, 3);
		int ref = RefAndPushBehaviour(L, entity, path);
		scene->SetComponent<c_Behaviour>(entity, path, ref);

		return 1;
	}
	else if (type == "vector")
	{
		float x = lua_tonumber(L, 3);
		float y = lua_tonumber(L, 4);
		float z = lua_tonumber(L, 5);

		scene->SetComponent<c_Vector>(entity, x, y, z);
	}

	return 0;
}

int Scene::lua_GetEntityCount(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int count = scene->GetEntityCount();
	lua_pushinteger(L, count);

	return 1;
}

int Scene::lua_IsEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	bool alive = scene->IsEntity(entity);
	lua_pushboolean(L, alive);

	return 1;
}

int Scene::lua_RemoveEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	scene->RemoveEntity(entity);

	return 0;
}

int Scene::lua_HasComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);
	bool hasComponent = true;

	if (type == "transform")
	{
		hasComponent = scene->HasComponents<c_Vector>(entity);
	}
	else if (type == "collision")
	{
		hasComponent = scene->HasComponents<c_Collision>(entity);
	}

	lua_pushboolean(L, hasComponent);

	return 1;
}

int Scene::lua_GetComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);

	if (!lua_isinteger(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushnil(L);
		return 1;
	}

	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);

	if (!scene->IsEntity(entity))
	{
		lua_pushnil(L);
		return 1;
	}


	if (type == "vector" && scene->HasComponents<c_Vector>(entity))
	{
		c_Vector& vector = scene->GetComponent<c_Vector>(entity);
		/*lua_pushvector(L, vector);*/
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

int Scene::lua_RemoveComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);

	int entity = lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);

	if (type == "vector" && scene->HasComponents<c_Vector>(entity))
	{
		scene->RemoveComponent<c_Vector>(entity);
		return 1;
	}
	return 0;
}

//Fuskbygge
entt::registry* Scene::GetRegistry()
{
	return &m_registry;
}

int Scene::RefAndPushBehaviour(lua_State* L, int entity, const char* path)
{
	luaL_dofile(L, path);

	lua_pushvalue(L, -1);
	int luaTableRef = luaL_ref(L, LUA_REGISTRYINDEX);

	lua_pushinteger(L, entity);
	lua_setfield(L, -2, "ID");

	lua_pushstring(L, path);
	lua_setfield(L, -2, "path");

	lua_getfield(L, -1, "OnCreate");
	lua_pushvalue(L, -2);
	lua_pcall(L, 1, 0, 0);

	return luaTableRef;
}