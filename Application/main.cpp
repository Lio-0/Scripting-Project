#include <iostream>
#include <thread>
#include <Windows.h>
#include <string>

#include "lua.hpp"

void DumpError(lua_State* L)
{
	//H�mta toppen av lua stacken och kolla om det �r en str�ng
	if (lua_gettop(L) > 0 && lua_isstring(L, -1))
	{
		std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;

		//Ta bort meddelandet fr�n stacken
		lua_pop(L, 1);
	}
}

//Function som k�rs parallelt av tr�d
void ConsoleThreadFunction(lua_State* L)
{

	//L�s console input
	std::string input;
	while (GetConsoleWindow())
	{
		std::cout << "> ";
		std::getline(std::cin, input);

		//F�rs�k k�ra str�ngen som lua kod
		if (luaL_dostring(L, input.c_str()) != LUA_OK)
		{
			DumpError(L);
		}
	}
}

int main()
{
	//Rekommenderat att ha ett men g�r att ha flera om det beh�vs
	lua_State* L = luaL_newstate();

	//�ppnar standardbibliotek f�r lua, g�r s� att kodstr�ngen g�r att k�ra
	luaL_openlibs(L);

	//Skapa tr�d
	std::thread consoleThread(ConsoleThreadFunction, L);

	bool running = true;
	while (running)
	{
		//Game & render loop
	}

	std::cout << "Hello world!" << std::endl;
	return 0;
}