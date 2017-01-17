#include "stdafx.h"

#include "LuaEngine.h"

//returns lua state object
lua_State* LuaEngine::L()
{
	return m_L;
}

//helper function to report errors in evaluated lua scripts
void LuaEngine::report_errors(int state)
{
	if (state != 0)
	{
		std::cerr << "ERR: " << lua_tostring(m_L, state) << std::endl;
		lua_pop(m_L, 1); //remove error
	}
}

//executes a lua script file
void LuaEngine::ExecuteFile(const char* file)
{
	if (file == NULL)
		return;

	int state = luaL_dofile(m_L, file);
	report_errors(state);
}

//execute a lua expression contained in string
//
//(could contain a full script with line breaks)
void LuaEngine::ExecuteString(const char* expression)
{
	if (expression == NULL)
	{
		std::cerr << "ERR: null expression passed to script engine!" << std::endl;
		return;
	}

	int state = luaL_dostring(m_L, expression);
	report_errors(state);
}