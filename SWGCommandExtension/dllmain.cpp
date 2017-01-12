// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <windows.h>
#include <detours.h>

#include "CreatureObject.h"
#include "CuiMediatorFactory.h"
#include "CuiChatParser.h"

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

DWORD globalDetailJmpAddress = 0x85E1C1;
DWORD globalDetailReturnJmp = 0x85E1C6;
float globalDetailOverrideValue = 12.0f;

/// Mid-Function Hooks
///
__declspec(naked) void globalDetailSliderHook()
{
	__asm
	{
		mov ebp, esp						// Do part of prologue we overwrote
		mov eax, [ebp + 0x8]				// We probably don't need this since we overwrite the value.
		mov eax, globalDetailOverrideValue	// Do our hack.
		jmp[globalDetailReturnJmp]			// Return to normal execution.
	}
}

DWORD terrainDistJmpAddress = 0x85E261;
DWORD terrainDistReturnJmp = 0x85E266;
float terrainDistanceOverrideValue = 40.0f;

__declspec(naked) void terrainDistanceSliderHook()
{
	__asm
	{
		mov ebp, esp							// Do part of prologue we overwrote
		mov eax, [ebp + 0x8]					// We probably don't need this since we overwrite the value.
		mov eax, terrainDistanceOverrideValue	// Do our hack.
		jmp[terrainDistReturnJmp]				// Return to normal execution.
	}
}
///
///

#define ATTACH_HOOK(X, Y) extern decltype(&X) Y; DetourAttach((PVOID*) &Y, X);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: {
		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		// Direct function hooks.

		ATTACH_HOOK(CuiChatParser::parse, oldChatParse);

		LONG errorCode = DetourTransactionCommit();

		if (errorCode == NO_ERROR) {
			//Detour successful

			const BYTE newData[7] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			writeBytes((BYTE*)0xC8D258, newData, 7);
			// Mid-function hooks for global detail and high detail terrain distance.
			writeJmp((BYTE*)terrainDistJmpAddress, (DWORD)terrainDistanceSliderHook, 5);
			writeJmp((BYTE*)globalDetailJmpAddress, (DWORD)globalDetailSliderHook, 5);
						
			// Show our loaded message (only displays if chat is already present).
			CuiChatParser::echo("[LOADED] Settings Override Extensions by N00854180T");
			CuiChatParser::echo("Use /exthelp for details on extension command usage.");
		} else {
			CuiChatParser::echo("[LOAD] FAILED");
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

