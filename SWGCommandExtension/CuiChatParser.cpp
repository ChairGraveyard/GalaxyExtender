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
#include "ClientProceduralTerrainAppearance.h"
#include "GameLanguageManager.h"
#include "FreeChaseCamera.h"
#include "TerrainObject.h"

//extern float terrainDistanceOverrideValue;
//extern float globalDetailOverrideValue;

/// General Utilities
///
template<typename String, typename Delimiter, typename Vector>
void split(const String& s, Delimiter delim, Vector& v) {
	auto i = 0;
	auto pos = s.find(delim);
	while (pos != String::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == String::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

float stof(const soe::unicode& str) {
	return std::stof(str.c_str());
}

bool CuiChatParser::parse(const soe::unicode& incomingCommand, soe::unicode& resultUnicode, uint32_t chatRoomID, bool useChatRoom) {
	// Create local string for convenience.
	soe::vector<soe::unicode> args;

	bool handled = false;
	auto groundScene = Game::getGroundScene();
	auto camera = groundScene ? groundScene->getFreeChaseCamera() : nullptr;

	// Make sure this is a slash command before interpreting.
	auto foundSlash = incomingCommand.find(L"/");
	if (foundSlash != soe::unicode::npos) {
		// Strip the slash.
		auto afterSlash = incomingCommand.substr(foundSlash + 1);

		// Break into arguments.
		split(afterSlash, L' ', args);

		if (args.size() >= 2) {
			auto &command = args[0];

			// Handle global detail command.
			if (command == L"globaldetail") {
				float newVal = stof(args[1]);
				float oldVal = TerrainObject::getLevelOfDetailThreshold();

				TerrainObject::setLevelOfDetailThreshold(newVal);

				float newValAfterSetting = TerrainObject::getLevelOfDetailThreshold();

				std::string values = "oldVal:" + std::to_string(oldVal) + " newValAfterSetting: " + std::to_string(newValAfterSetting);

				// Tell the user to slide the slider to make the override take effect.				
				Game::debugPrintUi(values.c_str());

				handled = true;
			} else if (command == L"highdetailterrain" || command == L"hdterrain") {
				float newVal = stof(args[1]);

				float oldVal = TerrainObject::getHighLevelOfDetailThreshold();

				TerrainObject::setHighLevelOfDetailThreshold(newVal);

				float newValAfterSetting = TerrainObject::getHighLevelOfDetailThreshold();

				std::string values = "oldVal:" + std::to_string(oldVal) + " newValAfterSetting: " + std::to_string(newValAfterSetting);

				// Tell the user to slide the slider to make the override take effect.												
				Game::debugPrintUi(values.c_str());

				handled = true;
			} else if (command == L"radialflora") {
				float newVal = stof(args[1]);
				ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(newVal);

				Game::debugPrintUi("Radial Flora distance set.");

				handled = true;
			} else if (command == L"noncollidableflora" || command == L"ncflora") {
				float newVal = stof(args[1]);
				ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(newVal);

				Game::debugPrintUi("Non-Collidable Flora distance set.");

				handled = true;
			} else if (command == L"viewdistance" || command == L"vd") {
				if (camera) {
					float newVal = stof(args[1]);

					camera->setViewDistance(newVal);

					Game::debugPrintUi("View distance distance set.");
				}

				handled = true;
			} else if (command == L"overrideall" || command == L"setall") {
				auto& setting = args[1];
				if (setting == L"help")
				{
					Game::debugPrintUi("This command sets the overrides for all settings at once.");
					Game::debugPrintUi("Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32");
					Game::debugPrintUi("Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50");
					Game::debugPrintUi("Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55");
					Game::debugPrintUi("High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64");
					Game::debugPrintUi("Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128");
				} else if (setting == L"default") {
					Game::debugPrintUi("Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32");

					if (camera)
						camera->setViewDistance(1024);

					TerrainObject::setLevelOfDetailThreshold(6);

					TerrainObject::setHighLevelOfDetailThreshold(10);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(64);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(32);
				} else if (setting == L"low") {
					Game::debugPrintUi("Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50");

					if (camera)
						camera->setViewDistance(1536);

					TerrainObject::setLevelOfDetailThreshold(8);

					TerrainObject::setHighLevelOfDetailThreshold(12);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(80);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(50);
				} else if (setting == L"medium" || setting == L"med") {
					Game::debugPrintUi("Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55");

					if (camera)
						camera->setViewDistance(2048);

					TerrainObject::setLevelOfDetailThreshold(9);
					
					TerrainObject::setHighLevelOfDetailThreshold(15);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(100);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(55);
				} else if (setting == L"high") {
					Game::debugPrintUi("High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64");

					if (camera)
						camera->setViewDistance(4096);

					TerrainObject::setLevelOfDetailThreshold(12);
					
					TerrainObject::setHighLevelOfDetailThreshold(30);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(128);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(64);
				} else if (setting == L"ultra") {
					Game::debugPrintUi("Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128");

					if (camera)
						camera->setViewDistance(4096);

					TerrainObject::setLevelOfDetailThreshold(16);
					
					TerrainObject::setHighLevelOfDetailThreshold(50);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(256);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(128);
				} else
					Game::debugPrintUi("No option specified. Syntax is /setall <default|low|medium|high|ultra> or /overrideall <default|low|medium|high|ultra");

				handled = true;
			}

			// Check for other commands
		} else if (afterSlash.size() > 0) {
			auto &command = afterSlash;

			if (command == L"assist2") {
				CreatureObject* creature = Game::getPlayerCreature();
				auto& lookAtTarget = creature->getLookAtTarget();
				Object* obj = lookAtTarget.getObject();

				if (obj) {
					CreatureObject* creo = obj->asCreatureObject();

					if (creo) {
						auto& newLookAtTarget = creo->getLookAtTarget();
						Object* newTargetObject = newLookAtTarget.getObject();

						if (newTargetObject) {
							ClientCommandQueue::enqueueCommand(soe::string::hashCode("target"), newTargetObject->getObjectID());
						}
					}
				}

				handled = 1;
			} if (command == L"testhooks") {
				Object* console = CuiMediatorFactory::get("DebugInfoPage");

				if (console != nullptr) {
					Game::debugPrintUi("activating console");

					CuiMediatorFactory::activate("DebugInfoPage");
				} else {
					Game::debugPrintUi("could not find console in cui mediator");
				}

				//ClientCommandQueue::enqueueCommand(soe::string::hashCode("target"));

				CreatureObject* creature = Game::getPlayerCreature();
				PlayerObject* playerObject = Game::getPlayerObject();
				soe::string strval = "testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing";
				soe::unicode unicodeTest = "testingUnicode";
				int mdasize = strval.size();
				int foundTestInVector = 0;

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
				foundTestInVector = findingVector != testingVector.end();

				testingVector.at(0) = "setter";

				GameLanguageManager::getLanguageSpeakSkillModName(2, strval);

				soe::string str2val = unicodeTest.toAscii();

				testingVector.pop_back();

				testingVector.emplace_back("newafterpop");

				if ("testingUnicode" == str2val) {
					str2val = "yaycompareworks";
				}

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
					sprintf_s(message, sizeof(message), "Your current health is: %d %s %s %s %d %d %lld %lld %d target: %lld %d %d %d %f",
						healthValue, strval.c_str(), str2val.c_str(), testingVector.at(0).c_str(), val1, val2, creoOID.get(), playOID,
						checkVal, targetOID.get(), atts.size(), mdasize, foundTestInVector, ClientProceduralTerrainAppearance::getStaticNonCollidableFloraDistance());

					Game::debugPrintUi(message);
				} else
				{
					Game::debugPrintUi("Main Player CreatureObject is null");
				}

				handled = true;
			}
			if (command == L"getcurrenthealth") {
				CreatureObject* creature = Game::getPlayerCreature();

				if (creature) {
					int healthValue = creature->getAttribute(CreatureObject::Health);

					char message[1024];
					sprintf_s(message, sizeof(message), "Your current health is: %d",
						healthValue);

					Game::debugPrintUi(message);
				}  else {
					Game::debugPrintUi("Main Player CreatureObject is null");
				}

				handled = true;
			} else if (command == L"getradialflora") {
				double radialDist = ClientProceduralTerrainAppearance::getDynamicNearFloraDistance();
				auto radialDistString = std::to_string(radialDist);

				Game::debugPrintUi("Radial Flora Distance: ");
				Game::debugPrintUi(radialDistString.c_str());

				handled = true;
			} else if (command == L"getnoncollidableflora" || command == L"getncflora") {
				double ncDist = ClientProceduralTerrainAppearance::getStaticNonCollidableFloraDistance();
				auto ncDistString = std::to_string(ncDist);

				Game::debugPrintUi("Non-Collidable Flora Distance: ");
				Game::debugPrintUi(ncDistString.c_str());

				handled = true;
			} else if (command == L"getviewdistance" || command == L"getvd") {
				if (camera) {
					auto viewDistString = std::to_string(camera->getViewDistance());

					Game::debugPrintUi("View/Rendering Distance: ");
					Game::debugPrintUi(viewDistString.c_str());
				}

				handled = true;
			} else if (command == L"exthelp") {
				Game::debugPrintUi("[COMMAND HELP] Settings Override Extensions by N00854180T");
				Game::debugPrintUi("/globaldetail X - sets the Global Detail Level to X, valid ranges are 1-24. You must move the slider in Terrain options AFTER using this command.");
				Game::debugPrintUi("<hdterrain|/highdetailterrain> X - sets the High Detail Terrain Distance to X, valid ranges are 1-50. You must move the slider in Terrain options AFTER using this command.");
				Game::debugPrintUi("/radialflora X - sets the Radial Flora Distance to X, valid ranges are 1-256.");
				Game::debugPrintUi("</ncflora|/noncollidableflora> X - sets the Non-Collidable Flora Distance to X, valid ranges are 1-128.");
				Game::debugPrintUi("</vd|/viewdistance> X - sets the View/Rendering Distance to X, valid ranges are 512-4096.");
				Game::debugPrintUi("</getvd|/getviewdistance> - Prints the current View/Rendering Distance value to the chat.");
				Game::debugPrintUi("/getradialflora - Prints the current Radial Flora Distance value to the chat.");
				Game::debugPrintUi("</getncflora|/getnoncollidableflora> - Prints the current Non-Collidable Flora Distance value to the chat.");
				Game::debugPrintUi("</setall|/overrideall> <default|low|medium|high|ultra> - Sets all graphics settings to preset values. Type /overrideall help for info on each preset.");
				Game::debugPrintUi("/exthelp - This command, which lists help info on available extension commands.");

				handled = true;
			}
		}

	}

	// If we handled the command above, don't call the original handler
	// and return 1 to indicate a successfully handled command.
	if (!handled)
		return originalParse::run(incomingCommand, resultUnicode, chatRoomID, useChatRoom);
	
	return true;
}