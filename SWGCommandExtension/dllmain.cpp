// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <windows.h>
#include <detours.h>

#include "CreatureObject.h"
#include "CuiMediatorFactory.h"
#include "CuiChatParser.h"
#include "Game.h"
#include "TerrainObject.h"
#include "SwgCuiLoginScreen.h"

using namespace std;

/// Memory Utilties
///
void writeJmp(BYTE* address, DWORD jumpTo, DWORD length) {
	DWORD oldProtect, newProtect, relativeAddress;

	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

	relativeAddress = (DWORD)(jumpTo - (DWORD)address) - 5;
	*address = 0xE9;
	*((DWORD *)(address + 0x1)) = relativeAddress;

	for (DWORD x = 0x5; x < length; x++)
	{
		*(address + x) = 0x90;
	}

	VirtualProtect(address, length, oldProtect, &newProtect);
}

void writeBytes(BYTE* address, const BYTE* values, int size) {
	DWORD oldProtect, newProtect;

	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(address, values, size);

	VirtualProtect(address, size, oldProtect, &newProtect);
}

#define ATTACH_HOOK(X, Y) extern GENERATE_HOOK_TYPE(X) Y; DetourAttach((PVOID*) &Y, X);
#define ATTACH_HOOK_THISCALL(CLASS, METHOD) DetourAttach((PVOID*)& ## CLASS ## _ ## METHOD ## _hook_t::original, ## CLASS ## _ ## METHOD ## _hook_t::fastHook);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: {
		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		// Direct function hooks.

		ATTACH_HOOK_THISCALL(SwgCuiLoginScreen, onButtonPressed);

		ATTACH_HOOK(CuiChatParser::parse, oldChatParse);
		ATTACH_HOOK(TerrainObject::setHighLevelOfDetailThresholdHook, oldSetHighLoDThreshold);
		ATTACH_HOOK(TerrainObject::setLevelOfDetailThresholdHook, oldSetLoDThreshold);

		LONG errorCode = DetourTransactionCommit();

		if (errorCode == NO_ERROR) {
			//Detour successful

			const BYTE newData[7] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			writeBytes((BYTE*)0xC8D258, newData, 7);
			// Mid-function hooks for global detail and high detail terrain distance.
						
			// Show our loaded message (only displays if chat is already present).
			Game::debugPrintUi("[LOADED] Settings Override Extensions by N00854180T");
			Game::debugPrintUi("Use /exthelp for details on extension command usage.");
		} else {
			Game::debugPrintUi("[LOAD] FAILED");
		}

		break;
	}
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourTransactionCommit();
		break;
	}

	return TRUE;
}

