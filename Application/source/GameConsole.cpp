#pragma once

#include "GameConsole.hpp"
#include <iostream>

void GameConsole::DumpError(lua_State* L)
{
	//Hämta toppen av lua stacken och kolla om det är en sträng
	if (lua_gettop(L) > 0 && lua_isstring(L, -1))
	{
		std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;

		//Ta bort meddelandet från stacken
		lua_pop(L, 1);
	}
}