#pragma once

#include <fstream>
#include <string>
#include "json.hpp"
#include "Scene.hpp"
#include "Components.hpp"
#include "GameConsole.hpp"
#include "lua.hpp"

class SceneSerializer {
public:
    static void Save(entt::registry& registry, const std::string& path);
    static void Load(lua_State* L, const std::string& path);

};
