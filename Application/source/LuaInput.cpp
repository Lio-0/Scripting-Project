#include "LuaInput.hpp"
#include "raylib.h"


LuaInput::LuaInput(lua_State* L) : m_luaState(L)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		{"GetKeyPressed", lua_GetKeyPressed},
		{"IsKeyDown", lua_IsKeyDown},
		{"IsKeyPressed", lua_IsKeyPressed},
		{"IsKeyPressedRepeat", lua_IsKeyPressedRepeat},
		{"IsKeyReleased", lua_IsKeyReleased},
		{"IsKeyUp", lua_IsKeyUp},
		{"GetMousePosition", lua_GetMousePosition},
		{"GetMouseDelta", lua_GetMouseDelta},
		{"GetMouseWheelMove", lua_GetMouseWheelMove},
		{"IsMouseButtonUp", lua_IsMouseButtonUp},
		{"IsMouseButtonDown", lua_IsMouseButtonDown},
		{NULL, NULL}
	};

	lua_pushlightuserdata(L, this);
	luaL_setfuncs(L, methods, 1);
	lua_setglobal(L, "input");

}

int LuaInput::lua_GetKeyPressed(lua_State* L)
{
	lua_pushinteger(L, GetKeyPressed());

	return 1;
}

int LuaInput::lua_IsKeyDown(lua_State* L)
{
	int key = lua_tointeger(L, 1);

	lua_pushboolean(L, IsKeyDown(key));

	return 1;
}

int LuaInput::lua_IsKeyPressed(lua_State* L)
{
	int key = lua_tointeger(L, 1);

	lua_pushboolean(L, IsKeyPressed(key));

	return 1;
}

int LuaInput::lua_IsKeyPressedRepeat(lua_State* L)
{
	int key = lua_tointeger(L, 1);

	lua_pushboolean(L, IsKeyPressedRepeat(key));

	return 1;
}

int LuaInput::lua_IsKeyReleased(lua_State* L)
{
	int key = lua_tointeger(L, 1);

	lua_pushboolean(L, IsKeyReleased(key));

	return 1;
}

int LuaInput::lua_IsKeyUp(lua_State* L)
{
	int key = lua_tointeger(L, 1);

	lua_pushboolean(L, IsKeyUp(key));

	return 1;
}

int LuaInput::lua_GetMousePosition(lua_State* L)
{
	Vector2 pos = GetMousePosition();

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);

	return 2;
}

int LuaInput::lua_GetMouseDelta(lua_State* L)
{
	Vector2 delta = GetMouseDelta();

	lua_pushnumber(L, delta.x);
	lua_pushnumber(L, delta.y);

	return 2;
}

int LuaInput::lua_GetMouseWheelMove(lua_State* L)
{
	float amount = GetMouseWheelMove();

	lua_pushnumber(L, amount);

	return 1;
}

int LuaInput::lua_IsMouseButtonUp(lua_State* L)
{
	int button = lua_tointeger(L, 1);

	lua_pushboolean(L, IsMouseButtonUp(button));

	return 1;
}

int LuaInput::lua_IsMouseButtonDown(lua_State* L)
{
	int button = lua_tointeger(L, 1);

	lua_pushboolean(L, IsMouseButtonDown(button));

	return 1;
}