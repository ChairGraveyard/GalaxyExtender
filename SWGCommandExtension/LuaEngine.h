#pragma once

#include <iostream>

#include <lua.hpp> //needed for Lua

/************************************************************************/
/*  LuaEngine class
/*
/*  implements a wrapper around the lua state object required
/*  to run user defined lua scripts. It is a non-copyable class
/*  as the application should only include one instance of the engine
/*  and execute scripts on that engine's state
/************************************************************************/
class LuaEngine
{
public:
	//Constructor
	LuaEngine(void) : m_L(luaL_newstate()) { luaL_openlibs(m_L); }
	LuaEngine(const LuaEngine& other);  //non-construction copy
	LuaEngine& operator=(const LuaEngine&); //non-copy
	~LuaEngine(void) { lua_close(m_L); } //Destructor clean-up

										 //returns lua state object
	lua_State* L();

	//Execute Lua File
	void ExecuteFile(const char* file);

	//Execute Lua Expression (contained in string)
	void ExecuteString(const char* expression);

private:
	//member lua state
	lua_State* m_L;

private:

	//handle errors
	void report_errors(int state);
};
