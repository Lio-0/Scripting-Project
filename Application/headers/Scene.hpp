#pragma once

#include "entt.hpp"
#include "lua.hpp"
#include "System.hpp"
#include <vector>
#include "Renderer.hpp"

class Scene
{
	entt::registry m_registry;
	lua_State* m_luaState;
	std::vector<System*> m_systems;
	
public:
	Scene(lua_State* L);
	~Scene() = default;

	//lua functions

	static void lua_openscene(lua_State* L, Scene* scene);

	//Entity Functions
	int GetEntityCount();
	int CreateEntity();
	bool IsEntity(int entity);
	void RemoveEntity(int entity);
	void DrawScene(Renderer& renderer, Camera& camera);

	//Systems

	template <typename T, typename...Args>
	void CreateSystem(Args... args);

	void UpdateSystems(float delta);

	//Components

	static int RefAndPushBehaviour(lua_State* L, int entity, const char* path);

	entt::registry* GetRegistry();

	template <typename...Args>
	bool HasComponents(int entity);

	template <typename T>
	T& GetComponent(int entity);

	template <typename T>
	void SetComponent(int entity, const T& component);

	template <typename T, typename...Args>
	void SetComponent(int entity, Args...args);

	template <typename T>
	void RemoveComponent(int entity);

private:

	//lua functions

	static int lua_CreateEntity(lua_State* L);

	static int lua_SetComponent(lua_State* L);

	static int lua_GetEntityCount(lua_State* L);

	static int lua_IsEntity(lua_State* L);

	static int lua_RemoveEntity(lua_State* L);

	static int lua_HasComponent(lua_State* L);

	static int lua_GetComponent(lua_State* L);

	static int lua_RemoveComponent(lua_State* L);
};

template<typename T, typename ...Args>
inline void Scene::CreateSystem(Args ...args)
{
	m_systems.emplace_back(new T(args...));
}

template<typename ... Args>
bool Scene::HasComponents(int entity)
{
	return m_registry.all_of<Args...>((entt::entity)entity);
}

template<typename T>
T& Scene::GetComponent(int entity)
{
	return m_registry.get<T>((entt::entity)entity);
}

template<typename T>
void Scene::SetComponent(int entity, const T& component)
{
	m_registry.emplace_or_replace<T>((entt::entity)entity, component);
}

template<typename T, typename ...Args>
void Scene::SetComponent(int entity, Args... args)
{
	m_registry.emplace_or_replace<T>((entt::entity)entity, args...);
}

template<typename T>
void Scene::RemoveComponent(int entity)
{
	m_registry.remove<T>((entt::entity)entity);
}
