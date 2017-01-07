// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "utility.h"
#include "LuaEngine.h"

#include "CreatureObject.h"

#include <windows.h>
#include <detours.h>
#include <iostream>
#include <iterator>


#include "LuaBridge\LuaBridge.h"

using namespace std;

/// Lua Related
static LuaEngine* lua = NULL;
///

/// Mid-Function Hooks
///
DWORD globalDetailJmpAddress = 0x85E1C1;
DWORD globalDetailReturnJmp = 0x85E1C6;
float globalDetailOverrideValue = 12.0f;

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

/// Direct Internal Functions
///
///
// Echo a message to the console.
#define ECHO_ADDRESS 0x424810
typedef void(*internalEcho)(const char*);
internalEcho echo = (internalEcho)ECHO_ADDRESS;

// Get Non-Collidable Flora Distance value.
#define GET_NONCOLLIDABLE_FLORA_ADDRESS 0x849310
typedef double(__cdecl* internalGetNonCollidableFloraDistance)();
internalGetNonCollidableFloraDistance getNonCollidableFloraDistance = (internalGetNonCollidableFloraDistance)GET_NONCOLLIDABLE_FLORA_ADDRESS;

// Get Radial Flora Distance value.
#define GET_RADIAL_FLORA_ADDRESS 0x8492B0
typedef double(__cdecl* internalGetRadialFloraDistance)();
internalGetRadialFloraDistance getRadialFloraDistance = (internalGetRadialFloraDistance)GET_RADIAL_FLORA_ADDRESS;

// Set the Non-Collidable Flora Distance value.
#define NONCOLLIDABLE_FLORA_ADDRESS 0x849300
typedef float(__cdecl* internalNonCollidableFloraSlider)(float);
internalNonCollidableFloraSlider nonCollidableFloraSlider = (internalNonCollidableFloraSlider)NONCOLLIDABLE_FLORA_ADDRESS;

// Set Radial Flora Distance value.
#define RADIAL_FLORA_ADDRESS 0x8492A0   
typedef float(__cdecl* internalRadialFloraSlider)(float);
internalRadialFloraSlider radialFloraSlider = (internalRadialFloraSlider)RADIAL_FLORA_ADDRESS;


#define GAME_GETPLAYER_ADDRESS 0x425140
typedef void*(__cdecl* internalGameGetPlayer)();
internalGameGetPlayer gameGetPlayer = (internalGameGetPlayer)GAME_GETPLAYER_ADDRESS; /* this returns the main player from the Game, usually its a CreatureObject*/

#define GAME_GETPLAYERCREATURE_ADDRESS 0x425200
typedef CreatureObject*(__cdecl* internalGameGetPlayerCreature)();
internalGameGetPlayerCreature gameGetPlayerCreature = (internalGameGetPlayerCreature)GAME_GETPLAYERCREATURE_ADDRESS; /* this returns the main CreatureObject from the Game*/

#define GAME_GETPLAYEROBJECT_ADDRESS 0x425180
typedef void*(__cdecl* internalGameGetPlayerObject)();
internalGameGetPlayerObject gameGetPlayerObject = (internalGameGetPlayerObject)GAME_GETPLAYEROBJECT_ADDRESS; /* this returns the main PlayerObject (ghost from CreatureObject) from the Game*/

///
///

/// Pointers/Offsets
///
#define VIEW_DISTANCE_BASE 0x01907F30
#define VIEW_DISTANCE_OFFSET 0xFC
///
///


/// Direct Hooks
///
// Technically this isn't THE command handler, it's just the handler
// for language commands, moods, etc., but it works for our case as 
// it's the last type of command checked, which means we know most others
// have gotten a chance to be processed before us.
#define COMMAND_HANDLER_ADDRESS 0x9FF6F0   
char(__cdecl* originalCommandHandler)(int, int, int, int);

char hkCommandHandler(int a1, int a2, int a3, int a4)
{
	// a1 is a pointer to our command string.
	DWORD* commandPointer = (DWORD*)a1;

	// Create local string for convenience.
	wstring tmpString((wchar_t*)(*commandPointer));
	wstring commandString(tmpString);
	vector<wstring> args;

	bool handled = false;

	// Make sure this is a slash command before interpreting.
	wstring::size_type foundSlash = commandString.find(L"/");
	if (foundSlash != wstring::npos)
	{
		// Strip the slash.
		wstring afterSlash = commandString.substr(foundSlash + 1);

		// Break into arguments.
		split(afterSlash, L' ', args);

		if (args.size() >= 2)
		{
			wstring &command = args[0];

			// Handle global detail command.
			if (command == L"globaldetail")
			{
				float newVal = stof(args[1]);

				globalDetailOverrideValue = newVal;

				// Tell the user to slide the slider to make the override take effect.				
				echo(("Global Detail Level set to " + to_string(newVal) + "! However, this setting will not take effect until you open the Options, click the Terrain tab, and move the Global Detail Level slider.").c_str());

				handled = true;
			}
			else if (command == L"highdetailterrain" || command == L"hdterrain")
			{
				float newVal = stof(args[1]);

				terrainDistanceOverrideValue = newVal;

				// Tell the user to slide the slider to make the override take effect.												
				echo(("High Detail Terrain Distance set to " + to_string(newVal) + "! However, this setting will not take effect until you open the Options, click the Terrain tab, and move the High Detail Terrain Distance slider.").c_str());

				handled = true;
			}
			else if (command == L"radialflora")
			{
				float newVal = stof(args[1]);
				radialFloraSlider(newVal);

				echo(("Radial Flora distance set to " + to_string(newVal)).c_str());

				handled = true;
			}
			else if (command == L"noncollidableflora" || command == L"ncflora")
			{
				float newVal = stof(args[1]);
				nonCollidableFloraSlider(newVal);

				echo(("Non-Collidable Flora distance set to " + to_string(newVal)).c_str());

				handled = true;
			}
			else if (command == L"viewdistance" || command == L"vd")
			{
				float newVal = stof(args[1]);

				DWORD* viewDistFirstPtr = (DWORD*)VIEW_DISTANCE_BASE;
				float* viewDistance = (float*)((*viewDistFirstPtr) + VIEW_DISTANCE_OFFSET);

				(*viewDistance) = newVal;

				echo(("View distance distance set to " + to_string(newVal)).c_str());

				handled = true;
			}
			else if (command == L"overrideall" || command == L"setall")
			{

				DWORD* viewDistFirstPtr = (DWORD*)VIEW_DISTANCE_BASE;
				float* viewDistance = (float*)((*viewDistFirstPtr) + VIEW_DISTANCE_OFFSET);

				echo("Note, you still must open Options -> Terrain and slide the Global Detail Level and High Detail Terrain Distance sliders for those settings to take effect.");

				wstring& setting = args[1];
				if (setting == L"help")
				{
					echo("This command sets the overrides for all settings at once.");
					echo("Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32");
					echo("Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50");
					echo("Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55");
					echo("High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64");
					echo("Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128");
				}
				else if (setting == L"default")
				{
					echo("Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32");

					(*viewDistance) = 1024;

					globalDetailOverrideValue = 6;
					terrainDistanceOverrideValue = 10;
					radialFloraSlider(64);
					nonCollidableFloraSlider(32);
				}
				else if (setting == L"low")
				{
					echo("Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50");

					(*viewDistance) = 1536;

					globalDetailOverrideValue = 8;
					terrainDistanceOverrideValue = 12;
					radialFloraSlider(80);
					nonCollidableFloraSlider(50);
				}
				else if (setting == L"medium" || setting == L"med")
				{
					echo("Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55");

					(*viewDistance) = 2048;

					globalDetailOverrideValue = 9;
					terrainDistanceOverrideValue = 15;
					radialFloraSlider(100);
					nonCollidableFloraSlider(55);
				}
				else if (setting == L"high")
				{
					echo("High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64");

					(*viewDistance) = 4096;

					globalDetailOverrideValue = 12;
					terrainDistanceOverrideValue = 30;
					radialFloraSlider(128);
					nonCollidableFloraSlider(64);
				}
				else if (setting == L"ultra")
				{
					echo("Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128");

					(*viewDistance) = 4096;

					globalDetailOverrideValue = 16;
					terrainDistanceOverrideValue = 50;
					radialFloraSlider(256);
					nonCollidableFloraSlider(128);
				}
				else
					echo("No option specified. Syntax is /setall <default|low|medium|high|ultra> or /overrideall <default|low|medium|high|ultra");

				handled = true;
			}
			else if (command == L"lua")
			{
				if (lua)
					lua->ExecuteString(ws2s(args[1]).c_str());

				handled = true;
			}
			else if (command == L"luaf")
			{
				if (lua)
					lua->ExecuteFile(ws2s(args[1]).c_str());

				handled = true;
			}

			// Check for other commands
		}
		else if (afterSlash.size() > 0)
		{
			wstring &command = afterSlash;

			if (command == L"getradialflora")
			{
				double radialDist = getRadialFloraDistance();
				echo(("Radial Flora Distance: " + to_string(radialDist)).c_str());

				handled = true;
			}
			else if (command == L"getnoncollidableflora" || command == L"getncflora")
			{
				double ncDist = getNonCollidableFloraDistance();
				echo(("Non-Collidable Flora Distance: " + to_string(ncDist)).c_str());
				
				handled = true;
			}
			else if (command == L"getviewdistance" || command == L"getvd")
			{
				DWORD* viewDistBase = (DWORD*)VIEW_DISTANCE_BASE;
				float* viewDistance = (float*)((*viewDistBase) + VIEW_DISTANCE_OFFSET);
				string viewDistString = to_string((*viewDistance));

				echo(("View/Rendering Distance: " + viewDistString).c_str());				

				handled = true;
			}
			else if (command == L"exthelp")
			{
				echo("[COMMAND HELP] Settings Override Extensions by N00854180T");
				echo("/globaldetail X - sets the Global Detail Level to X, valid ranges are 1-24. You must move the slider in Terrain options AFTER using this command.");
				echo("<hdterrain|/highdetailterrain> X - sets the High Detail Terrain Distance to X, valid ranges are 1-50. You must move the slider in Terrain options AFTER using this command.");
				echo("/radialflora X - sets the Radial Flora Distance to X, valid ranges are 1-256.");
				echo("</ncflora|/noncollidableflora> X - sets the Non-Collidable Flora Distance to X, valid ranges are 1-128.");
				echo("</vd|/viewdistance> X - sets the View/Rendering Distance to X, valid ranges are 512-4096.");
				echo("</getvd|/getviewdistance> - Prints the current View/Rendering Distance value to the chat.");
				echo("/getradialflora - Prints the current Radial Flora Distance value to the chat.");
				echo("</getncflora|/getnoncollidableflora> - Prints the current Non-Collidable Flora Distance value to the chat.");
				echo("</setall|/overrideall> <default|low|medium|high|ultra> - Sets all graphics settings to preset values. Type /overrideall help for info on each preset.");
				echo("/exthelp - This command, which lists help info on available extension commands.");

				handled = true;
			}
			else if (command == L"getcurrenthealth")
			{
				CreatureObject* creature = gameGetPlayerCreature();

				if (creature)
					echo(string("Your current health is: " + to_string(creature->getAttribute(CreatureObject::Health))).c_str());			
				else
					echo("Main Player CreatureObject is null");				

				handled = true;
			}
			else if (command == L"getcurrentlocation" || command == L"getloc")
			{
				CreatureObject* creature = gameGetPlayerCreature();

				if (creature)											
					echo(string("Your location is: " + to_string(creature->getXLocation()) + " " + to_string(creature->getZLocation())).c_str());
				else
					echo("Main Player CreatureObject is null");

				handled = true;
			}

		}

	}

	// If we handled the command above, don't call the original handler
	// and return 1 to indicate a successfully handled command.
	if (handled)
		return '1';
	else
		return originalCommandHandler(a1, a2, a3, a4);
}
///
///


BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:

		// Direct function hooks.
		originalCommandHandler = (char(__cdecl*)(int, int, int, int))DetourFunction((PBYTE)COMMAND_HANDLER_ADDRESS, (PBYTE)hkCommandHandler);		
		
		// Mid-function hooks for global detail and high detail terrain distance.
		writeJmp((BYTE*)terrainDistJmpAddress, (DWORD)terrainDistanceSliderHook, 5);
		writeJmp((BYTE*)globalDetailJmpAddress, (DWORD)globalDetailSliderHook, 5);

		// Show our loaded message (only displays if chat is already present).
		echo("[LOADED] GalaxyExtender by N00854180T");
		echo("Use /exthelp for details on extension command usage.");

		// Load Lua Engine
		lua = new LuaEngine();

		// Register other internal classes for Lua use here.
		if (lua)
			CreatureObject::register_lua(lua->L());		

		luabridge::getGlobalNamespace(lua->L()).addFunction("echo", echo);

		luabridge::getGlobalNamespace(lua->L()).addFunction("gameGetPlayerCreature", gameGetPlayerCreature);
		luabridge::getGlobalNamespace(lua->L()).addFunction("gameGetPlayer", gameGetPlayer);
		luabridge::getGlobalNamespace(lua->L()).addFunction("gameGetPlayerObject", gameGetPlayerObject);
		
		break;
	}

	return TRUE;
}

