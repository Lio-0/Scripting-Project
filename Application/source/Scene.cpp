#pragma once

#include "Scene.hpp"
#include "Components.hpp"
#include "raylib.h"

#define SCRIPT_PATH "scripts\\"

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
		{"HasComponent", lua_HasComponent},
		{"GetComponent", lua_GetComponent},
		{"RemoveComponent", lua_RemoveComponent},
		{"RemoveEntity", lua_RemoveEntity},
		{"GetEntityCount", lua_GetEntityCount},
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

void lua_pushvector(lua_State* L, const c_Vector& vec)
{
	lua_newtable(L);
	lua_pushnumber(L, vec.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, vec.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, vec.z);
	lua_setfield(L, -2, "z");
}

void lua_pushtransform(lua_State* L, const c_Transform& transform)
{
	lua_newtable(L);

	lua_pushvector(L, transform.position);
	lua_setfield(L, -2, "position");

	lua_pushvector(L, transform.rotation);
	lua_setfield(L, -2, "rotation");

	lua_pushvector(L, transform.scale);
	lua_setfield(L, -2, "scale");
}

c_Vector lua_getvector(lua_State* L, int index)
{
	c_Vector vec;
	lua_getfield(L, index, "x");
	vec.x = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "y");
	vec.y = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "z");
	vec.z = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return vec;
}

c_Transform lua_totransform(lua_State* L, int index)
{
	c_Transform t;

	lua_getfield(L, index, "position");
	t.position = lua_getvector(L, lua_gettop(L));
	lua_pop(L, 1);

	lua_getfield(L, index, "rotation");
	t.rotation = lua_getvector(L, lua_gettop(L));
	lua_pop(L, 1);

	lua_getfield(L, index, "scale");
	t.scale = lua_getvector(L, lua_gettop(L));
	lua_pop(L, 1);

	return t;
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
	else if (type == "vector")
	{
		if (!lua_istable(L, 3)) {
			return luaL_error(L, "Expected table for vector");
		}

		c_Vector vector = lua_getvector(L, 3);
		scene->SetComponent<c_Vector>(entity, vector);
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
	else if (type == "transform")
	{
		if (!lua_istable(L, 3)) {
			return luaL_error(L, "Expected table for transform");
		}

		c_Transform transform = lua_totransform(L, 3);
		scene->SetComponent<c_Transform>(entity, transform);
	}
	else if (type == "visual")
	{
		const char* modelName = lua_tostring(L, 3);
		const char* textureName = lua_tostring(L, 4);
		bool isRendered = lua_toboolean(L, 5);
		scene->SetComponent<c_Visual>(entity, modelName, textureName, isRendered);
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
		lua_pushvector(L, vector);
		return 1;
	}
	else if (type == "transform" && scene->HasComponents<c_Transform>(entity))
	{
		c_Transform& transform = scene->GetComponent<c_Transform>(entity);
		lua_pushtransform(L, transform);
		return 1;
	}
	else if (type == "health" && scene->HasComponents<c_Health>(entity))
	{
		c_Health& health = scene->GetComponent<c_Health>(entity);
		lua_pushnumber(L, health.Value);
		return 1;
	}
	else if (type == "poison" && scene->HasComponents<c_Poison>(entity))
	{
		c_Poison& poison = scene->GetComponent<c_Poison>(entity);
		lua_pushnumber(L, poison.TickDamage);
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

void Scene::DrawScene(Renderer& renderer)
{
	std::vector<RenderData> renderObjects;

	auto view = m_registry.view<c_Transform, c_Visual>();

	view.each([&](const c_Transform& transform, const c_Visual& visual)
	{
		if (!visual.isRendered)
			return;

		RenderData data;
		data.position = { transform.position.x, transform.position.y, transform.position.z };
		data.rotation = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
		data.scale = { transform.scale.x, transform.scale.y, transform.scale.z };
		data.modelName = visual.modelName;
		data.textureName = visual.textureName;

		renderObjects.push_back(data);
	});

	renderer.Draw(renderObjects);
}
