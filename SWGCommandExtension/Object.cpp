#include "stdafx.h"
#include "Object.h"

#include "CreatureObject.h"

#define SOE_RTDYNAMIC_CAST_ADDRESS 0x0131E6F1;

Object::dyn_cast_t BaseHookedObject::soe_rt_dynamic_cast_func = (dyn_cast_t)SOE_RTDYNAMIC_CAST_ADDRESS;

PVOID BaseHookedObject::dynamicCast(PVOID inptr, const PVOID SrcType, const PVOID TargetType) {
	/* this calls the ___RTDynamicCast that the client uses so we can cast between their objects with their own typeinfo data*/
	return soe_rt_dynamic_cast_func(inptr, 0, SrcType, TargetType, 0);
}

bool Object::isCreatureObject() {
	return asCreatureObject() != nullptr;
}

CreatureObject* Object::asCreatureObject() {
	return reinterpret_cast<CreatureObject*>(dynamicCast(this, (PVOID) Object::RTTI, (PVOID) CreatureObject::RTTI));
}