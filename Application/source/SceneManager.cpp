#pragma once
#include "SceneManager.hpp"

void SceneManager::AddScene(Scene* scene, std::string name)
{
	m_scenes.emplace(scene, name);
}

int SceneManager::LuaLoadScene(lua_State* L)
{
	std::string name = lua_tostring(L, 1);
	Scene::lua_openscene(L, m_scenes.at(name));
	return 0;
}