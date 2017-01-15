#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <tuple>
#include "NetworkId.h"
#include "Transform.h"

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

#define PTR_TO_OBJECT_TYPEINFO 0x01868A84

class CreatureObject;
class Controller;
class CellProperty;

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

template<std::size_t, typename> struct Hook;

template<std::size_t Address, class C, class R, class... Args>
struct Hook<Address, R(C::*)(Args...)> {
	typedef R(C::*newMethod_t)(Args...);

	static R __thiscall callHook(C* object, Args... args) {
		return (object->*(hookStorage_t::newMethod))(args...);
	}

	typedef decltype(&callHook) original_t;

	typedef HookStorage<Address, newMethod_t, original_t> hookStorage_t;

	static R run(C* thisPointer, Args... args) {
		return hookStorage_t::original(thisPointer, args...);
	}

};

template<std::size_t Address, class R, class... Args>
struct Hook<Address, R(*)(Args...)> {
	typedef R(*newMethod_t)(Args...);

	static R callHook(Args... args) {
		return hookStorage_t::newMethod(args...);
	}

	typedef decltype(&callHook) original_t;

	typedef HookStorage<Address, newMethod_t, original_t> hookStorage_t;

	static R run(Args... args) {
		return hookStorage_t::original(args...);
	}
};

class BaseHookedObject {
public:
	template<typename T>
	const T& getMemoryReference(int offset) const {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	template<typename T>
	T& getMemoryReference(int offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	template<int FunctionAddress, typename Return, typename ... ArgumentTypes>
	Return runMethod(ArgumentTypes ... args) {
		return ThisCall<FunctionAddress, Return, decltype(this), ArgumentTypes...>::run(this, args...);
	}

	template<int FunctionAddress, typename Return, typename ... ArgumentTypes>
	Return runMethod(ArgumentTypes ... args) const {
		return ThisCall<FunctionAddress, Return, decltype(this), ArgumentTypes...>::run(this, args...);
	}

	template<int FunctionAddress, typename Return, typename ... ArgumentTypes>
	static Return runStatic(ArgumentTypes ... args) {
		return Call<FunctionAddress, Return, ArgumentTypes...>::run(args...);
	}

	template<uint32_t VirtualOffset, typename Return, typename ... ArgumentTypes>
	Return runVirtual(ArgumentTypes ... args) {
		return ThisCall<VirtualOffset, Return, decltype(this), ArgumentTypes...>::runVirtual(this, args...);
	}

	template<uint32_t VirtualOffset, typename Return, typename ... ArgumentTypes>
	Return runVirtual(ArgumentTypes ... args) const {
		return ThisCall<VirtualOffset, Return, decltype(this), ArgumentTypes...>::runVirtual(this, args...);
	}

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

class Object : public BaseHookedObject {
public:
	constexpr static uint32_t RTTI = PTR_TO_OBJECT_TYPEINFO;

	NetworkId& getObjectID() {
		return getMemoryReference<NetworkId>(0x20);
	}

	const NetworkId& getObjectID() const {
		return getMemoryReference<const NetworkId>(0x20);
	}

	Controller* getController() {
		return getMemoryReference<Controller*>(0x2C);
	}

	const Transform& getTransform_o2p() const {
		return getMemoryReference<Transform>(0x50);
	}

	Transform& getTransform_o2p() {
		return getMemoryReference<Transform>(0x50);
	}

	Vector getPosition_p() const {
		return getTransform_o2p().getPosition_p();
	}

	void positionAndRotationChanged(bool val, const Vector& oldPosition) {
		runVirtual<0x58, void, bool, const Vector&>(val, oldPosition);
	}

	void setTransform_o2p(const Transform& newObjectToParentTransform) {
		auto oldPosition = getPosition_p();

		getTransform_o2p() = newObjectToParentTransform;

		positionAndRotationChanged(false, oldPosition);
	}

	void setPosition_p(const Vector& position) {
		auto oldPosition = getPosition_p();

		getTransform_o2p().setPosition_p(position);

		positionAndRotationChanged(false, oldPosition);
	}

	Object* getAttachedTo() {
		return getMemoryReference<Object*>(0x34);
	}

	void setParentCell(CellProperty* cell) {
		runVirtual<0x34, void>(cell);
	}

	CellProperty* getCellProperty() {
		return runMethod<0x00B24540, CellProperty*>();
	}

	void removeFromWorld() {
		runVirtual<0xC, void>();
	}

	bool isCreatureObject();
	CreatureObject* asCreatureObject();

};
