#include "Scene.hpp"
#include "Components.hpp"


int Scene::GetEntityCount()
{
	int count = 0;

	for (auto entity: m_registry.view<entt::entity>())
	{
		count++;
	}

	return count;
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

}

void Scene::lua_openscene(lua_State* L, Scene* scene)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		{"CreateEntity", lua_CreateEntity},
		{"SetComponent", lua_SetComponent},
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

void lua_pushtransform(lua_State* L, const Transform& transform)
{
	lua_newtable(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, transform.x);
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, transform.y);
	lua_settable(L, -3);

	lua_pushstring(L, "rotation");
	lua_pushnumber(L, transform.rotation);
	lua_settable(L, -3);
}

int Scene::lua_CreateEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	scene->CreateEntity();
	return 0;
}

int Scene::lua_SetComponent(lua_State* L)
{
	return 0;
}

int Scene::lua_GetEntityCount(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int count = scene->GetEntityCount();
	lua_pushinteger(L, count);

	return 1;
}

int Scene::lua_isEntity(lua_State* L)
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
		hasComponent = scene->HasComponents<Transform>(entity);
	}
	else if (type == "collision")
	{
		hasComponent = scene->HasComponents<Collision>(entity);
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


	if (type == "transform" && scene->HasComponents<Transform>(entity))
	{
		Transform& transform = scene->GetComponent<Transform>(entity);
		lua_pushtransform(L, transform);
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

	if (type == "transform" && scene->HasComponents<Transform>(entity))
	{
		scene->RemoveComponent<Transform>(entity);
		return 1;
	}
	return 0;
}


//Fuskbygge
entt::registry* Scene::GetRegistry()
{
	return &m_registry;
}