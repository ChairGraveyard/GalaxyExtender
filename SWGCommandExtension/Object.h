#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdint>

#include "LuaEngine.h"
#include "LuaBridge\LuaBridge.h"

#ifdef NDEBUG
/*	
 *	allows redirect to client code using push/ret trick cause masm cant jmp to absolute values
 *  also this preserves the stack and registers so no need to repush all values with __thiscall in Release builds
*/
#define JUMPTOCLIENT(addyval) __asm	\
	{							\
		__asm push addyval		\
		__asm ret				\
	}
#endif

class CreatureObject;

/*
 *
 * this allows doing __thiscalls, first param is <this> pointer that is passed into <ecx> of the specified function by the address
 *
 */
template<uint32_t FunctionAddress, typename Return, typename This, typename ... ArgumentTypes>
class ThisCall {
public:
	typedef Return(__thiscall* client_func_t)(This, ArgumentTypes...);

	static inline Return run(This thisPointer, ArgumentTypes ... args) {
		static client_func_t func = (client_func_t)FunctionAddress;
		return func(thisPointer, args...);
	}
};

class Object {
protected:
	template<typename T>
	const T& getMemoryReference(int offset) const {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	template<typename T>
	T& getMemoryReference(int offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

public:
	uint64_t& getObjectID() {
		return getMemoryReference<uint64_t>(0x20);
	}

	const uint64_t& getObjectID() const {
		return getMemoryReference<uint64_t>(0x20);
	}

	bool isCreatureObject();
	CreatureObject* asCreatureObject();

	typedef PVOID(__cdecl* dyn_cast_t)(
		PVOID inptr,
		LONG VfDelta,
		PVOID SrcType,
		PVOID TargetType,
		BOOL isReference
		);

	static dyn_cast_t soe_rt_dynamic_cast_func;
	static PVOID dynamicCast(PVOID inptr, PVOID SrcType, PVOID TargetType);

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua 
			.beginNamespace("Game") //our defined namespace (w.e we want to call it) 
			.beginClass<Object>("Object") //define class object 
			//.addConstructor<void(*)(void)>() //reg. empty constructor 
			.addFunction("isCreatureObject", &Object::isCreatureObject)
			.addFunction("asCreatureObject", &Object::asCreatureObject)			
			.endClass() //end class 
			.endNamespace(); //end namespace
	}
};
