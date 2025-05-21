#pragma once

#include "lua.hpp"


class LuaInput
{
	lua_State* m_luaState;

public:
	LuaInput(lua_State* L);
	~LuaInput() = default;

private:

	static int lua_GetKeyPressed(lua_State* L);
	static int lua_IsKeyDown(lua_State* L);
	static int lua_IsKeyPressed(lua_State* L);
	static int lua_IsKeyPressedRepeat(lua_State* L);
	static int lua_IsKeyReleased(lua_State* L);
	static int lua_IsKeyUp(lua_State* L);

	static int lua_GetMousePosition(lua_State* L);
	static int lua_GetMouseDelta(lua_State* L);
	static int lua_GetMouseWheelMove(lua_State* L);
	static int lua_IsMouseButtonUp(lua_State* L);
	static int lua_IsMouseButtonDown(lua_State* L);

	static int lua_DisableCursor(lua_State* L);
	static int lua_EnableCursor(lua_State* L);
};
