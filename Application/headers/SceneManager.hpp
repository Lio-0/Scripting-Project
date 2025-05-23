#pragma once
#include "Scene.hpp"
#include <unordered_map>

enum PlayerState {
	Alive = 0,
	Dead = 1,
	Winning = 2
};

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
	void UpdateTimers(float delta);
private:
	static int lua_ResetScene(lua_State* L);
	static int lua_LoadScene(lua_State* L);
	static int lua_GetScreenDim(lua_State* L);
	static int lua_Win(lua_State* L);
	static int lua_Lose(lua_State* L);
	static int lua_GetPState(lua_State* L);
	static int lua_ToggleBlockChoice(lua_State* L);
	static std::unordered_map<std::string, Scene*> m_scenes;
	static Scene* m_currentScene;
	static bool m_changeScene;
	static bool m_blockMenu;
	static float m_winTimer;
	static float m_loseTimer;
	static PlayerState m_playerState;
};