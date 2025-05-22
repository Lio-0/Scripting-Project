#pragma once
#include "SceneManager.hpp"
#include "SceneSerializer.hpp"
#include "entt.hpp"
std::unordered_map<std::string, Scene*> SceneManager::m_scenes;
Scene* SceneManager::m_currentScene;
bool SceneManager::m_changeScene = false;
float SceneManager::m_winTimer = 1;
float SceneManager::m_loseTimer = 1;
PlayerState SceneManager::m_playerState = Alive;

SceneManager::SceneManager(lua_State* L)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		{"LoadScene", lua_LoadScene},
		{"GetScreenDim", lua_GetScreenDim},
		{"ResetScene", lua_ResetScene},
		{"Win", lua_Win},
		{"Lose", lua_Lose},
		{"PlayerState", lua_GetPState},
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
	if (m_currentScene == m_scenes["editor"])
	{
		SceneSerializer::Save(*m_currentScene->GetRegistry(), "assets/Level1.json");
	}
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
		if (m_currentScene == m_scenes["game"]) 
		{
			m_currentScene->ClearEntities();
			if (luaL_dofile(L, "scripts/initLevel.lua") != LUA_OK)
			{
				GameConsole::DumpError(L);
			}
			SceneSerializer::Load(L, "assets/Level1.json");
		}
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

int SceneManager::lua_Win(lua_State* L)
{
	m_winTimer = 0;
	m_playerState = Winning;

	return 0;
}

int SceneManager::lua_Lose(lua_State* L)
{
	m_loseTimer = 0;
	m_playerState = Dead;

	return 0;
}

int SceneManager::lua_GetPState(lua_State* L)
{
	lua_pushinteger(L, m_playerState);
	return 1;
}

void SceneManager::UpdateTimers(float delta)
{
	if (m_winTimer < 1)
	{
		m_winTimer += delta;
		DrawText("YOU WIN!", GetScreenWidth() / 2 - 250, GetScreenHeight() / 2, 100, GREEN);
	}
	else if (m_loseTimer < 1)
	{
		m_loseTimer += delta;
		DrawText("YOU LOSE!", GetScreenWidth() / 2 - 275, GetScreenHeight() / 2, 100, RED);
	}
	else
	{
		m_playerState = Alive;
	}
}