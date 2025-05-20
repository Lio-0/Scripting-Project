#pragma once
#include "Scene.hpp"
#include <unordered_map>

class SceneManager
{
public:
	SceneManager(lua_State* L);
	~SceneManager() = default;
	void AddScene(Scene* scene, std::string name);
	static int LuaLoadScene(lua_State* L);
private:
	static std::unordered_map<std::string, Scene*> m_scenes;
};

SceneManager::SceneManager(lua_State* L)
{
	lua_register(L, "LoadScene", LuaLoadScene);
}