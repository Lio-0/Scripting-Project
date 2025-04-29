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