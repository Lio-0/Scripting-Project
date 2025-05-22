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

void Scene::ClearEntities() {
	auto view = m_registry.view<entt::entity>();

	for (auto [entity] : view.each())
	{
		m_registry.destroy(entity);
	}
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

void lua_pushcolor(lua_State* L, const c_Color& col)
{
	lua_newtable(L);
	lua_pushnumber(L, col.a);
	lua_setfield(L, -2, "a");
	lua_pushnumber(L, col.r);
	lua_setfield(L, -2, "r");
	lua_pushnumber(L, col.g);
	lua_setfield(L, -2, "g");
	lua_pushnumber(L, col.b);
	lua_setfield(L, -2, "b");
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

c_Color lua_tocolor(lua_State* L, int index)
{
	c_Color color;

	lua_getfield(L, index, "r");
	color.r = (unsigned char)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "g");
	color.g = (unsigned char)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "b");
	color.b = (unsigned char)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "a");
	color.a = (unsigned char)luaL_optinteger(L, -1, 255); // default to 255
	lua_pop(L, 1);

	return color;
}


c_Button lua_tobutton(lua_State* L, int index)
{
	c_Button btn;

	lua_getfield(L, index, "label");
	btn.label = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "posX");
	btn.posX = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "posY");
	btn.posY = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "width");
	btn.width = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "height");
	btn.height = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "textColour");
	btn.textColour = lua_tocolor(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "textPosX");
	btn.textPosX = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "textPosY");
	btn.textPosY = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "fontSize");
	btn.fontSize = lua_tointeger(L, -1);
	lua_pop(L, 1);

	return btn;
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

	int entity = (int)lua_tointeger(L, 1);
	std::string type = lua_tostring(L, 2);

	// Handle marker components early — no data needed
	if (type == "clickable") {
		scene->SetComponent<c_Clickable>(entity);
		return 0;
	}
	else if (type == "selected") {
		scene->SetComponent<c_Selected>(entity);
		return 0;
	}

	// Now check that the value (3rd argument) exists for data components
	if (lua_gettop(L) < 3) {
		return luaL_error(L, "SetComponent: Missing value for component type '%s'", type.c_str());
	}


	if (type == "vector")
	{
		if (!lua_istable(L, 3)) {
			return luaL_error(L, "Expected table for vector");
		}

		c_Vector vector = lua_getvector(L, 3);
		scene->SetComponent<c_Vector>(entity, vector);
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
	else if (type == "camera")
	{
		int ID = (int)lua_tointeger(L, 3);
		c_Vector offset = lua_getvector(L, 4);
		c_Vector target = lua_getvector(L, 5);
		scene->SetComponent<c_Camera>(entity, ID, offset, target);
	}
	else if (type == "collision")
	{
		int layer = (int)lua_tointeger(L, 3);
		scene->SetComponent<c_Collision>(entity, layer);
	}
	else if (type == "collectible")
	{
		int ID = (int)lua_tointeger(L, 3);
		bool collected = lua_toboolean(L, 4);
		scene->SetComponent<c_Collectible>(entity, ID, collected);
	}
	else if (type == "color")
	{
		if (!lua_istable(L, 3)) {
			return luaL_error(L, "Expected table for color");
		}

		c_Color color = lua_tocolor(L, 3);
		scene->SetComponent<c_Color>(entity, color);
	}
	else if (type == "button")
	{
		if (!lua_istable(L, 3))
		{
			return luaL_error(L, "Expected table for button");
		}

		c_Button btn = lua_tobutton(L, 3);
		scene->SetComponent<c_Button>(entity, btn);
	}
	else if (type == "goal")
	{
		bool open = lua_toboolean(L, 3);
		scene->SetComponent<c_Goal>(entity, open);
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
	int entity = (int)lua_tointeger(L, 1);
	bool alive = scene->IsEntity(entity);
	lua_pushboolean(L, alive);

	return 1;
}

int Scene::lua_RemoveEntity(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = (int)lua_tointeger(L, 1);
	scene->RemoveEntity(entity);

	return 0;
}

int Scene::lua_HasComponent(lua_State* L)
{
	Scene* scene = lua_GetSceneUpValue(L);
	int entity = (int)lua_tointeger(L, 1);
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
	else if (type == "vector")
	{
		hasComponent = scene->HasComponents<c_Vector>(entity);
	}
	else if (type == "camera")
	{
		hasComponent = scene->HasComponents<c_Camera>(entity);
	}
	else if (type == "collectible")
	{
		hasComponent = scene->HasComponents<c_Collectible>(entity);
	}
	else if (type == "button")
	{
		hasComponent = scene->HasComponents<c_Button>(entity);
	}
	else if (type == "color")
	{
		hasComponent = scene->HasComponents<c_Color>(entity);
	}
	else if (type == "goal")
	{
		hasComponent = scene->HasComponents<c_Goal>(entity);
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

	int entity = (int)lua_tointeger(L, 1);
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
	else if (type == "camera" && scene->HasComponents<c_Camera>(entity))
	{
		c_Camera& camera = scene->GetComponent<c_Camera>(entity);

		lua_newtable(L);

		lua_pushnumber(L, camera.ID);
		lua_setfield(L, -2, "ID");

		lua_pushvector(L, camera.positionOffset);
		lua_setfield(L, -2, "offset");

		lua_pushvector(L, camera.target);
		lua_setfield(L, -2, "target");

		return 1;
	}
	else if (type == "collision" && scene->HasComponents<c_Collision>(entity))
	{
		c_Collision& collision = scene->GetComponent<c_Collision>(entity);

		lua_pushnumber(L, collision.layer);

		return 1;
	}
	else if (type == "color" && scene->HasComponents<c_Color>(entity))
	{
		c_Color& color = scene->GetComponent<c_Color>(entity);

		lua_pushcolor(L, color);

		return 1;
	}
	else if (type == "button" && scene->HasComponents<c_Button>(entity))
	{
		c_Button& btn = scene->GetComponent<c_Button>(entity);

		lua_newtable(L);

		lua_pushinteger(L, btn.posX);
		lua_setfield(L, -2, "posX");

		lua_pushinteger(L, btn.posY);
		lua_setfield(L, -2, "posY");

		lua_pushinteger(L, btn.width);
		lua_setfield(L, -2, "width");

		lua_pushinteger(L, btn.height);
		lua_setfield(L, -2, "height");

		lua_pushstring(L, btn.label.c_str());
		lua_setfield(L, -2, "label");

		lua_pushcolor(L, btn.textColour);
		lua_setfield(L, -2, "textColour");

		lua_pushinteger(L, btn.fontSize);
		lua_setfield(L, -2, "fontSize");

		lua_pushinteger(L, btn.textPosX);
		lua_setfield(L, -2, "textPosX");

		lua_pushinteger(L, btn.textPosY);
		lua_setfield(L, -2, "textPosY");

		return 1;
	}
	if (type == "goal" && scene->HasComponents<c_Goal>(entity))
	{
		c_Goal& goal = scene->GetComponent<c_Goal>(entity);
		lua_pushboolean(L, goal.open);
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

void Scene::DrawScene(Renderer& renderer, Camera& camera)
{
	std::vector<RenderData> renderObjects;

	auto view = m_registry.view<c_Transform, c_Visual, c_Color>();

	view.each([&](const c_Transform& transform, const c_Visual& visual, const c_Color& color)
	{
		if (!visual.isRendered)
			return;

		RenderData data;
		data.position = { transform.position.x, transform.position.y, transform.position.z };
		data.rotation = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
		data.scale = { transform.scale.x, transform.scale.y, transform.scale.z };
		data.modelName = visual.modelName;
		data.textureName = visual.textureName;
		data.r = color.r;
		data.g = color.g;
		data.b = color.b;
		data.a = color.a;

		renderObjects.push_back(data);
	});

	std::vector<UIData> uiObjects;
	auto uiView = m_registry.view<c_Color, c_Button>();

	uiView.each([&](const c_Color& color, const c_Button& button)
		{
			if (!button.active)
				return;

			UIData data(color.r, color.g, color.b, color.a, 
				button.label, button.posX, button.posY, button.width, button.height, 
				button.textColour.r, button.textColour.g, button.textColour.b, button.textColour.a, 
				button.textPosX, button.textPosY, button.fontSize);

			uiObjects.push_back(data);
		});

	BeginMode3D(camera);
	renderer.Draw(renderObjects);
	EndMode3D();

	renderer.DrawUI(uiObjects);
}

void Scene::Reset()
{
	auto view = m_registry.view<c_Behaviour>();

	view.each([&](c_Behaviour& script) {
			lua_rawgeti(m_luaState, LUA_REGISTRYINDEX, script.LuaTableRef);
			lua_getfield(m_luaState, -1, "OnReset");
			lua_pushvalue(m_luaState, -2);
			lua_pushnumber(m_luaState, GetFrameTime());

			if (lua_pcall(m_luaState, 2, 0, 0) != LUA_OK)
			{
				GameConsole::DumpError(m_luaState);
			}
			lua_pop(m_luaState, 1);
		});
}