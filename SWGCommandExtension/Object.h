#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <tuple>
#include "NetworkId.h"

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
		static client_func_t func = reinterpret_cast<client_func_t>(FunctionAddress);
		return func(thisPointer, args...);
	}

	static inline Return runVirtual(This thisPointer, ArgumentTypes ... args) {
		auto vtable = *reinterpret_cast<uint32_t**>(thisPointer);
		client_func_t func = reinterpret_cast<client_func_t>(vtable[(FunctionAddress / sizeof(uint32_t*))]);
		return func(thisPointer, args...);
	}
};

template<uint32_t FunctionAddress, typename Return, typename ... ArgumentTypes>
class Call {
public:
	static inline Return run(ArgumentTypes ... args) {
		typedef Return(__cdecl* client_func_t)(ArgumentTypes...);

		static client_func_t func = reinterpret_cast<client_func_t>(FunctionAddress);
		return func(args...);
	}
};

template<typename ThisType, typename C, typename ReturnType, typename ... Args>
struct HookThisOld {
	static C newMethod;

	static ReturnType __fastcall fastHook(ThisType thisPointer, void*, Args... args) {
		ThisType object = (ThisType) thisPointer;
		return (object->*(HookThisOld<ThisType, C, ReturnType, Args...>::newMethod))(args...);
	}

	typedef decltype(&fastHook) original_t;

	static original_t original;

	static ReturnType runOriginal(ThisType thisPointer, Args... args) {
		return original(thisPointer, 0, args...);
	}
};

template<std::size_t Address, class newMethod_t, class original_t> 
class HookStorage {
public:
	static constexpr std::size_t address = Address;

	static newMethod_t newMethod;
	static original_t original;
};

template<std::size_t Address, class newMethod_t, class original_t>
newMethod_t HookStorage<Address, newMethod_t, original_t>::newMethod;

template<std::size_t Address, class newMethod_t, class original_t>
original_t HookStorage<Address, newMethod_t, original_t>::original = (original_t) Address;

template<std::size_t, typename> struct HookThis;

template<std::size_t Address, class C, class R, class... Args>
struct HookThis<Address, R(C::*)(Args...)> {
	using return_type = R;

	static constexpr std::size_t arity = sizeof...(Args);

	template <std::size_t N>
	struct argument {
		static_assert(N < arity, "error: invalid parameter index.");
		using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
	};

	typedef R(C::*newMethod_t)(Args...);

	static R __thiscall thiscallHook(C* object, Args... args) {
		return (object->*(hookStorage_t::newMethod))(args...);
	}

	typedef decltype(&thiscallHook) original_t;

	typedef HookStorage<Address, newMethod_t, original_t> hookStorage_t;

	static R run(C* thisPointer, Args... args) {
		return hookStorage_t::original(thisPointer, args...);
	}

};

class Object {
public:
	template<typename T>
	const T& getMemoryReference(int offset) const {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	template<typename T>
	T& getMemoryReference(int offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	template<uint32_t FunctionAddress, typename Return, typename ... ArgumentTypes>
	Return runMethod(ArgumentTypes ... args) {
		return ThisCall<FunctionAddress, Return, Object*, ArgumentTypes...>::run(this, args...);
	}

	template<uint32_t VirtualOffset, typename Return, typename ... ArgumentTypes>
	Return runVirtual(ArgumentTypes ... args) {
		return ThisCall<VirtualOffset, Return, Object*, ArgumentTypes...>::runVirtual(this, args...);
	}

	NetworkId& getObjectID() {
		return getMemoryReference<NetworkId>(0x20);
	}

	const NetworkId& getObjectID() const {
		return getMemoryReference<const NetworkId>(0x20);
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
	static PVOID dynamicCast(PVOID inptr, const PVOID SrcType, const PVOID TargetType);
};
