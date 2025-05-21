#pragma once
#include "SceneManager.hpp"

std::unordered_map<std::string, Scene*> SceneManager::m_scenes;
Scene* SceneManager::m_currentScene;
bool SceneManager::m_changeScene = false;

SceneManager::SceneManager(lua_State* L)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		{"LoadScene", lua_LoadScene},
		{"GetScreenDim", lua_GetScreenDim},
		{"ResetScene", lua_ResetScene},
		{NULL, NULL}
	};

	lua_pushlightuserdata(L, this);
	luaL_setfuncs(L, methods, 1);
	lua_setglobal(L, "system");
}

void SceneManager::AddScene(Scene* scene, std::string name)
{
	m_scenes.emplace(name, scene);
}

int SceneManager::lua_LoadScene(lua_State* L)
{	
	m_currentScene = m_scenes[lua_tostring(L, 1)];
	m_changeScene = true;
	return 0;
}

void SceneManager::UpdateScene(lua_State* L, float delta)
{
	if (m_changeScene)
	{
		Scene::lua_openscene(L, m_currentScene);
		m_currentScene->Reset();
		m_changeScene = false;
	}
	m_currentScene->UpdateSystems(delta);
}

void SceneManager::DrawScene(Renderer& renderer, Camera& camera)
{
	m_currentScene->DrawScene(renderer, camera);
}

void SceneManager::LoadScene(lua_State* L, std::string name)
{
	m_currentScene = m_scenes[name];
	Scene::lua_openscene(L, m_currentScene);
}

Scene* SceneManager::GetCurrentScene()
{
	return m_currentScene;
}

int SceneManager::lua_GetScreenDim(lua_State* L)
{
	lua_pushnumber(L, GetScreenWidth());
	lua_pushnumber(L, GetScreenHeight());
	return 2;
}

int SceneManager::lua_ResetScene(lua_State* L)
{
	m_currentScene->Reset();
	return 0;
}
