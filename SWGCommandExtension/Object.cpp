#include "stdafx.h"
#include "Object.h"

#define PTR_TO_OBJECT_TYPEINFO (void*) 0x01868A84
#define PTR_TO_CREATUREOBJECT_TYPINFO (void*) 0x01869148
#define SOE_RTDYNAMIC_CAST_ADDRESS 0x0131E6F1;

Object::dyn_cast_t Object::soe_rt_dynamic_cast_func = (dyn_cast_t)SOE_RTDYNAMIC_CAST_ADDRESS;

PVOID Object::dynamicCast(PVOID inptr, PVOID SrcType, PVOID TargetType) {
	/* this calls the ___RTDynamicCast that the client uses so we can cast between their objects with their own typeinfo data*/
	return soe_rt_dynamic_cast_func(inptr, 0, SrcType, TargetType, 0);
}

bool Object::isCreatureObject() {
	return asCreatureObject() != NULL;
}

CreatureObject* Object::asCreatureObject() {
	return (CreatureObject*) dynamicCast(this, PTR_TO_OBJECT_TYPEINFO, PTR_TO_CREATUREOBJECT_TYPINFO);
}