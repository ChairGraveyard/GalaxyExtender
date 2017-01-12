#include "stdafx.h"
#include <string>

#include <windows.h>
#include <iostream>
#include <iterator>
#include <vector>

#include "CuiChatParser.h"
#include "CuiMediatorFactory.h"
#include "Game.h"
#include "ClientCommandQueue.h"

float globalDetailOverrideValue = 12.0f;

/// Pointers/Offsets
///
#define VIEW_DISTANCE_BASE 0x01907F30
#define VIEW_DISTANCE_OFFSET 0xFC
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

#define GAMELANGUAGEMANAGER_GETLANGUAGESPEAKSKILLMODNAME_ADDRESS 0x011C6270
typedef void(__cdecl* getLanguageSpeakSkillModName_t)(const int, soe::string&);
getLanguageSpeakSkillModName_t getLanguageSpeakSkillModName = (getLanguageSpeakSkillModName_t)GAMELANGUAGEMANAGER_GETLANGUAGESPEAKSKILLMODNAME_ADDRESS;


#define LOGINSCREEN_ACTIVATE 0xC8D190
typedef void(__thiscall* loginScreenActivate_t)(void*);
loginScreenActivate_t originalLoginScreenActivateObject = (loginScreenActivate_t)LOGINSCREEN_ACTIVATE; //i patch this one on startup, no hooks needed

#define LOGINSCREEN_BUTTON_PRESSED 0xC8D460
typedef void(__thiscall* loginScreenOnButtonPressed_t)(void*, void*);
loginScreenOnButtonPressed_t originalLoginScreenOnButtonPressedObject = (loginScreenOnButtonPressed_t)LOGINSCREEN_BUTTON_PRESSED;

#define GAME_APPLICATION_ADDRES 0x01908858

void __fastcall hkLoginScreenOnButtonPressed(Object* thisPointer, void* unused, void* uicontext) {
	/*char message[128];
	int* gameType = (int*)GAME_APPLICATION_ADDRES;

	sprintf_s(message, "this:%p uicontext:%p applicationType:%d", thisPointer, uicontext, *gameType);
	OutputDebugStringA(message);*/

	originalLoginScreenOnButtonPressedObject(thisPointer, uicontext);
}

/// General Utilities
///
void split(const std::string& s, char delim, std::vector<std::string>& v) {
	auto i = 0;
	auto pos = s.find(delim);
	while (pos != std::string::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

void split(const std::wstring& s, wchar_t delim, std::vector<std::wstring>& v) {
	auto i = 0;
	auto pos = s.find(delim);
	while (pos != std::wstring::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == std::wstring::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

DEFINE_HOOOK(COMMAND_HANDLER_ADDRESS, CuiChatParser::parse, oldChatParse);

bool CuiChatParser::parse(const soe::unicode& incomingCommand, soe::unicode& resultUnicode, uint32_t chatRoomID, bool useChatRoom) {
	// Create local string for convenience.
	using namespace std;
	wstring commandString(incomingCommand.c_str());
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
				echo("Global Detail Level changed! However, this setting will not take effect until you open the Options, click the Terrain tab, and move the Global Detail Level slider.");

				handled = true;
			} else if (command == L"highdetailterrain" || command == L"hdterrain")
			{
				float newVal = stof(args[1]);

				extern float terrainDistanceOverrideValue;

				terrainDistanceOverrideValue = newVal;

				// Tell the user to slide the slider to make the override take effect.												
				echo("High Detail Terrain Distance changed! However, this setting will not take effect until you open the Options, click the Terrain tab, and move the High Detail Terrain Distance slider.");

				handled = true;
			} else if (command == L"radialflora")
			{
				float newVal = stof(args[1]);
				radialFloraSlider(newVal);

				echo("Radial Flora distance set.");

				handled = true;
			} else if (command == L"noncollidableflora" || command == L"ncflora")
			{
				float newVal = stof(args[1]);
				nonCollidableFloraSlider(newVal);

				echo("Non-Collidable Flora distance set.");

				handled = true;
			} else if (command == L"viewdistance" || command == L"vd")
			{
				float newVal = stof(args[1]);

				DWORD* viewDistFirstPtr = (DWORD*)VIEW_DISTANCE_BASE;
				float* viewDistance = (float*)((*viewDistFirstPtr) + VIEW_DISTANCE_OFFSET);

				(*viewDistance) = newVal;

				echo("View distance distance set.");

				handled = true;
			} else if (command == L"overrideall" || command == L"setall")
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
				} else if (setting == L"default")
				{
					echo("Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32");

					(*viewDistance) = 1024;

					globalDetailOverrideValue = 6;
					extern float terrainDistanceOverrideValue;

					terrainDistanceOverrideValue = 10;
					radialFloraSlider(64);
					nonCollidableFloraSlider(32);
				} else if (setting == L"low")
				{
					echo("Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50");

					(*viewDistance) = 1536;

					globalDetailOverrideValue = 8;
					extern float terrainDistanceOverrideValue;

					terrainDistanceOverrideValue = 12;
					radialFloraSlider(80);
					nonCollidableFloraSlider(50);
				} else if (setting == L"medium" || setting == L"med")
				{
					echo("Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55");

					(*viewDistance) = 2048;

					globalDetailOverrideValue = 9;
					extern float terrainDistanceOverrideValue;

					terrainDistanceOverrideValue = 15;
					radialFloraSlider(100);
					nonCollidableFloraSlider(55);
				} else if (setting == L"high")
				{
					echo("High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64");

					(*viewDistance) = 4096;

					globalDetailOverrideValue = 12;
					extern float terrainDistanceOverrideValue;

					terrainDistanceOverrideValue = 30;
					radialFloraSlider(128);
					nonCollidableFloraSlider(64);
				} else if (setting == L"ultra")
				{
					echo("Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128");

					(*viewDistance) = 4096;

					globalDetailOverrideValue = 16;
					extern float terrainDistanceOverrideValue;

					terrainDistanceOverrideValue = 50;
					radialFloraSlider(256);
					nonCollidableFloraSlider(128);
				} else
					echo("No option specified. Syntax is /setall <default|low|medium|high|ultra> or /overrideall <default|low|medium|high|ultra");

				handled = true;
			}

			// Check for other commands
		} else if (afterSlash.size() > 0)
		{
			wstring &command = afterSlash;

			if (command == L"assist2")
			{
				Object* console = CuiMediatorFactory::get("DebugInfoPage");

				if (console != nullptr) {
					echo("activating console");

					CuiMediatorFactory::activate("DebugInfoPage");
				} else {
					echo("could not find console in cui mediator");
				}

				//				ClientCommandQueue::enqueueCommand(soe::string::hashCode("target"));

				CreatureObject* creature = Game::getPlayerCreature();
				auto& lookAtTarget = creature->getLookAtTarget();
				Object* obj = lookAtTarget.getObject();

				if (obj) {
#ifndef NDEBUG
					echo("obj not null");
#endif
					CreatureObject* creo = obj->asCreatureObject();

					if (creo) {
#ifndef NDEBUG
						echo("creo not null");
#endif
						auto& newLookAtTarget = creo->getLookAtTarget();
						Object* newTargetObject = newLookAtTarget.getObject();

						if (newTargetObject) {
#ifndef NDEBUG
							echo("newTargetObject not null");
#endif
							ClientCommandQueue::enqueueCommand(soe::string::hashCode("target"), newTargetObject->getObjectID());
						}
					}
				}

				handled = 1;
			}
			if (command == L"getcurrenthealth")
			{
				CreatureObject* creature = Game::getPlayerCreature();
				PlayerObject* playerObject = Game::getPlayerObject();
				soe::string strval = "testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing";
				soe::unicode unicodeTest = "testingUnicode";
				int mdasize = strval.size();
				int foundShitInVector = 0;

				soe::vector<soe::string> testingVector = { "asdagdsg", "balsda", "asdjasd", "asoidhaoisf" };
				testingVector.emplace_back("mdaaa");
				testingVector.emplace_back("balsda");
				testingVector.emplace_back("echostringtest");
				testingVector.emplace_back("echostringtest2");
				testingVector.emplace_back("echostringtest3");
				testingVector.emplace_back("echostringtest4");
				testingVector.emplace_back("echostringtest5");
				testingVector.emplace_back("echostringtest6");
				testingVector.emplace_back("echostringtest8");

				auto findingVector = testingVector.find("echostringtest4");
				foundShitInVector = findingVector != testingVector.end();

				testingVector.at(0) = "setter";

				getLanguageSpeakSkillModName(2, strval);

				soe::string str2val = unicodeTest.toAscii();

				testingVector.pop_back();

				testingVector.emplace_back("newafterpop");

				if ("testingUnicode" == str2val) {
					str2val = "yaycompareworks";
				}

				//soe::unicode testingTheEmptyOne = *emptyUnicodeString;
				str2val = str2val + "addition";

				if (creature)
				{
					const soe::vector<int> atts = creature->getAttributesArray(); //this intentionally copies the incoming vector to test
					int healthValue = creature->getAttribute(CreatureObject::Health);
					bool val1 = playerObject->speaksLanguage(1);
					bool val2 = playerObject->speaksLanguage(2);
					auto& creoOID = creature->getObjectID();
					uint64_t playOID = playerObject->getObjectID();
					ClientObject* ghost = creature->getEquippedObject("ghost");
					int checkVal = playOID == ghost->getObjectID();
					auto lookAtTarget = creature->getLookAtTarget();
					auto& targetOID = lookAtTarget.getObjectID();

					char message[1024];
					sprintf_s(message, sizeof(message), "Your current health is: %d %s %s %s %d %d %lld %lld %d target: %lld %d %d %d",
						healthValue, strval.c_str(), str2val.c_str(), testingVector.at(0).c_str(), val1, val2, creoOID.get(), playOID,
						checkVal, targetOID.get(), atts.size(), mdasize, foundShitInVector);

					echo(message);

					//clientCommandQueueEnqueue(stlportstring::hashCode("target"), &creature->getObjectID(), emptyUnicodeString);
				} else
				{
					echo("Main Player CreatureObject is null");
				}

				handled = true;
			} else if (command == L"getradialflora")
			{
				double radialDist = getRadialFloraDistance();
				string radialDistString = to_string(radialDist);

				echo("Radial Flora Distance: ");
				echo(radialDistString.c_str());

				handled = true;
			} else if (command == L"getnoncollidableflora" || command == L"getncflora")
			{
				double ncDist = getNonCollidableFloraDistance();
				string ncDistString = to_string(ncDist);

				echo("Non-Collidable Flora Distance: ");
				echo(ncDistString.c_str());

				handled = true;
			} else if (command == L"getviewdistance" || command == L"getvd")
			{
				DWORD* viewDistBase = (DWORD*)VIEW_DISTANCE_BASE;
				float* viewDistance = (float*)((*viewDistBase) + VIEW_DISTANCE_OFFSET);
				string viewDistString = to_string((*viewDistance));

				echo("View/Rendering Distance: ");
				echo(viewDistString.c_str());

				handled = true;
			} else if (command == L"exthelp")
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
		}

	}

	// If we handled the command above, don't call the original handler
	// and return 1 to indicate a successfully handled command.
	if (!handled)
		return (*oldChatParse)(incomingCommand, resultUnicode, chatRoomID, useChatRoom);
	
	return true;
}