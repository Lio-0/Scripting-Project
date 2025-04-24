#pragma once
#include "lua.hpp"

class GameConsole
{
public:
	GameConsole() = default;
	~GameConsole() = default;
	static void DumpError(lua_State* L);
private:

};