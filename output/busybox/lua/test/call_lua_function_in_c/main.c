#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


/* Lua interpreter */
lua_State *L;

int lua_add(int x, int y)
{
	int sum;
	// function name
	lua_getglobal(L, "add");
	// first argument
	lua_pushnumber(L, x);
	// second argument
	lua_pushnumber(L, y);
	// call function
	lua_call(L, 2, 1);
	// get result
	sum = (int)lua_tonumber(L, -1);
	// cleanup the return
	lua_pop(L, 1);
	return sum;
}

int main(int argc, char *argv[])
{
	int sum;
	// initialize Lua
	L = luaL_newstate(); 	// lua 5.3
	//L = lua_open();  		// lua 5.0
	// local Lua base libraries
	luaL_openlibs(L);
	// load script
	luaL_dofile(L, "mymath.lua");
	// call function
	sum = lua_add(10, 15);
	// print result
	printf("The sum is %d\n", sum);
	// cleanup Lua
	lua_close(L);
	return 0;
}
