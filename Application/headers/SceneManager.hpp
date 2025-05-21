#pragma once
#include "Scene.hpp"
#include <unordered_map>

class SceneManager
{
public:
	SceneManager(lua_State* L);
	~SceneManager() = default;
	void AddScene(Scene* scene, std::string name);
	void UpdateScene(lua_State* L, float delta);
	void DrawScene(Renderer& renderer, Camera& camera);
	void LoadScene(lua_State* L, std::string name);
	Scene* GetCurrentScene();
private:
	static int lua_ResetScene(lua_State* L);
	static int lua_LoadScene(lua_State* L);
	static int lua_GetScreenDim(lua_State* L);
	static std::unordered_map<std::string, Scene*> m_scenes;
	static Scene* m_currentScene;
	static bool m_changeScene;
};