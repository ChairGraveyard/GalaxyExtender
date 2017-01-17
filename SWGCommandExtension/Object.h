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
		return func(thisPointer, std::forward<ArgumentTypes>(args)...);
	}

	static inline Return runVirtual(This thisPointer, ArgumentTypes ... args) {
		auto vtable = *reinterpret_cast<uint32_t**>(thisPointer);
		client_func_t func = reinterpret_cast<client_func_t>(vtable[(FunctionAddress / sizeof(uint32_t*))]);
		return func(thisPointer, std::forward<ArgumentTypes>(args)...);
	}
};

template<uint32_t FunctionAddress, typename Return, typename ... ArgumentTypes>
class Call {
public:
	static inline Return run(ArgumentTypes... args) {
		typedef Return(__cdecl* client_func_t)(ArgumentTypes...);

		static client_func_t func = reinterpret_cast<client_func_t>(FunctionAddress);
		return func(std::forward<ArgumentTypes>(args)...);
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
		return (object->*(hookStorage_t::newMethod))(std::forward<Args>(args)...);
	}

	typedef decltype(&callHook) original_t;

	typedef HookStorage<Address, newMethod_t, original_t> hookStorage_t;

	static R run(C* thisPointer, Args... args) {
		return hookStorage_t::original(thisPointer, std::forward<Args>(args)...);
	}

};

template<std::size_t Address, class R, class... Args>
struct Hook<Address, R(*)(Args...)> {
	typedef R(*newMethod_t)(Args...);

	static R callHook(Args... args) {
		return hookStorage_t::newMethod(std::forward<Args>(args)...);
	}

	typedef decltype(&callHook) original_t;

	typedef HookStorage<Address, newMethod_t, original_t> hookStorage_t;

	static R run(Args... args) {
		return hookStorage_t::original(std::forward<Args>(args)...);
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
		return ThisCall<FunctionAddress, Return, decltype(this), ArgumentTypes...>::run(this, std::forward<ArgumentTypes>(args)...);
	}

	template<int FunctionAddress, typename Return, typename ... ArgumentTypes>
	Return runMethod(ArgumentTypes ... args) const {
		return ThisCall<FunctionAddress, Return, decltype(this), ArgumentTypes...>::run(this, std::forward<ArgumentTypes>(args)...);
	}

	template<int FunctionAddress, typename Return, typename ... ArgumentTypes>
	static Return runStatic(ArgumentTypes... args) {
		return Call<FunctionAddress, Return, ArgumentTypes...>::run(std::forward<ArgumentTypes>(args)...);
	}

	template<uint32_t VirtualOffset, typename Return, typename ... ArgumentTypes>
	Return runVirtual(ArgumentTypes ... args) {
		return ThisCall<VirtualOffset, Return, decltype(this), ArgumentTypes...>::runVirtual(this, std::forward<ArgumentTypes>(args)...);
	}

	template<uint32_t VirtualOffset, typename Return, typename ... ArgumentTypes>
	Return runVirtual(ArgumentTypes ... args) const {
		return ThisCall<VirtualOffset, Return, decltype(this), ArgumentTypes...>::runVirtual(this, std::forward<ArgumentTypes>(args)...);
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

	/*template<class T, class ...Args>
	static T* create_hooked_object(Args... args) {
		//auto allocator = std::allocator<T>();
		T* address = reinterpret_cast<T*>(::operator new(sizeof(T)));

		address->ctor(std::forward<Args>(args)...);
		//new (&(*address)) T(std::forward<Args>(args)...);

		return address;
	}*/

	template<class T, class ...Args>
	static T* create_soe_object_new(Args&&... args) {
		T* address = reinterpret_cast<T*>(::operator new(sizeof(T)));

		address->ctor(std::forward<Args>(args)...);

		return address;
	}

	template<class T, class ...Args>
	static T* create_soe_object(Args&&... args) {
		T* address = soe::allocator<T>::allocate(sizeof(T));

		address->ctor(std::forward<Args>(args)...);

		return address;
	}
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

	soe::vector<Object*>* getAttachedObjects() {
		return getMemoryReference<soe::vector<Object*>*>(0x38);
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

	void setTransform_o2p(const Transform& newObjectToParentTransform);
	void setPosition_p(const Vector& position);

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

	bool isChildObject() {
		return runMethod<0x00B22310, bool>();
	}

	bool getKill() {
		return runMethod<0x00B22320, bool>();
	}

	soe::string getDebugInformation(bool parent = true) const {
		soe::string info;
		
		runMethod<0x00B223B0, void , soe::string&, bool>(info, parent);

		return info;
	}

	void addToWorld() {
		runMethod<0x00B225F0, void>();
	}

	bool isInWorldCell() const {
		return runMethod<0x00B22BE0, bool>();
	}

	const Transform& getTransform_o2w() const {
		return runMethod<0x00B22C80, const Transform&>();
	}

	void setTransform_o2w(const Transform& trans) {
		runMethod<0x00B22CC0, void, const Transform&>(trans);
	}

	void setTransform_a2w(const Transform& trans) const {
		runMethod<0x0B22E1, void, const Transform&>(trans);
	}

	const Transform& getTransform_a2w() const {
		return runMethod<0x00B22E90, const Transform&>();
	}

	Transform getTransform_o2c() const {
		Transform tr;

		runMethod<0x00B22F00, void, Transform&>(tr);

		return tr;
	}

	void setAppearance(Object* appearance) {
		runMethod<0x00B22F60, void>(appearance);
	}

	Object* stealAppearance() {
		return runMethod<0x00B22FF0, Object*>();
	}

	const Vector& getAppearanceSphereCenter() const {
		return runMethod<0x00B23020, const Vector&>();
	}

	int getNumberOfAttachedObjects() const {
		return runMethod<0x00B232E0, int>();
	}

	Object* getAttachedObject(int idx) {
		return runMethod<0x00B23300, Object*>(idx);
	}

	Object* getChildObject(int idx) {
		return runMethod<0x00B23360, Object*>(idx);
	}

	//virtual method at 0x38?
	void detachFromObject(int flags) {
		runMethod<0x00B23670, void>(flags);
	}

	const char* getObjectTemplateName() const {
		return runMethod<0x00B23C40, const char*>();
	}
};
