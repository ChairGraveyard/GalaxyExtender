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
#include "Controller.h"
#include "CellProperty.h"
#include "CollisionWorld.h"
#include "ObjectAttributeManager.h"
#include "SwgCuiConsole.h"

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

	static bool consoleActive = false;

	if (consoleActive)
		resultUnicode += (L"\\#ffffff > \\#888888" + incomingCommand + L"\\#ffffff\n");

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
			if (command == L"octrl") {
				CreatureObject* creature = Game::getPlayerCreature();
				auto controller = creature->getController();

				float msg = stof(args[1]);
				float value = args.size() > 2 ? stof(args[2]) : 0;

				if (controller) {
					controller->appendMessage(static_cast<int>(msg), value, 0);
				} else {
					resultUnicode += L"null controller";

					return true;
				}

				handled = true;
			} else if (command == L"globaldetail") {
				float newVal = stof(args[1]);
				float oldVal = TerrainObject::getLevelOfDetailThreshold();

				TerrainObject::setLevelOfDetailThreshold(newVal);

				float newValAfterSetting = TerrainObject::getLevelOfDetailThreshold();

				std::string values = "oldVal:" + std::to_string(oldVal) + " newValAfterSetting: " + std::to_string(newValAfterSetting);

				resultUnicode += values.c_str();

				handled = true;
			} else if (command == L"highdetailterrain" || command == L"hdterrain") {
				float newVal = stof(args[1]);

				float oldVal = TerrainObject::getHighLevelOfDetailThreshold();

				TerrainObject::setHighLevelOfDetailThreshold(newVal);

				float newValAfterSetting = TerrainObject::getHighLevelOfDetailThreshold();

				std::string values = "oldVal:" + std::to_string(oldVal) + " newValAfterSetting: " + std::to_string(newValAfterSetting);

				resultUnicode += values.c_str();

				handled = true;
			} else if (command == L"radialflora") {
				float newVal = stof(args[1]);
				ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(newVal);

				resultUnicode += L"Radial Flora distance set.";

				handled = true;
			} else if (command == L"noncollidableflora" || command == L"ncflora") {
				float newVal = stof(args[1]);
				ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(newVal);

				resultUnicode += L"Non-Collidable Flora distance set.";

				handled = true;
			} else if (command == L"viewdistance" || command == L"vd") {
				if (camera) {
					float newVal = stof(args[1]);

					camera->setViewDistance(newVal);

					resultUnicode += L"View distance distance set.";
				}

				handled = true;
			} else if (command == L"overrideall" || command == L"setall") {
				auto& setting = args[1];
				if (setting == L"help")
				{
					resultUnicode += L"This command sets the overrides for all settings at once.\n";
					resultUnicode += L"Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32\n";
					resultUnicode += L"Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50\n";
					resultUnicode += L"Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55\n";
					resultUnicode += L"High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64\n";
					resultUnicode += L"Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128\n";
				} else if (setting == L"default") {
					resultUnicode +=  L"Default - View Distance: 1024, Global Detail: 6, High Detail Terrain: 10, Radial Flora: 64, Non-Collidable Flora: 32\n";

					if (camera)
						camera->setViewDistance(1024);

					TerrainObject::setLevelOfDetailThreshold(6);

					TerrainObject::setHighLevelOfDetailThreshold(10);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(64);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(32);
				} else if (setting == L"low") {
					resultUnicode += "Low - View Distance: 1536, Global Detail: 8, High Detail Terrain: 12, Radial Flora: 80, Non-Collidable Flora: 50";

					if (camera)
						camera->setViewDistance(1536);

					TerrainObject::setLevelOfDetailThreshold(8);

					TerrainObject::setHighLevelOfDetailThreshold(12);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(80);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(50);
				} else if (setting == L"medium" || setting == L"med") {
					resultUnicode += L"Medium - View Distance: 2048, Global Detail: 9, High Detail Terrain: 15, Radial Flora: 100, Non-Collidable Flora: 55";

					if (camera)
						camera->setViewDistance(2048);

					TerrainObject::setLevelOfDetailThreshold(9);
					
					TerrainObject::setHighLevelOfDetailThreshold(15);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(100);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(55);
				} else if (setting == L"high") {
					resultUnicode += L"High - View Distance: 4096, Global Detail: 12, High Detail Terrain: 30, Radial Flora: 128, Non-Collidable Flora: 64";

					if (camera)
						camera->setViewDistance(4096);

					TerrainObject::setLevelOfDetailThreshold(12);
					
					TerrainObject::setHighLevelOfDetailThreshold(30);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(128);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(64);
				} else if (setting == L"ultra") {
					resultUnicode += L"Ultra - View Distance: 4096, Global Detail: 16, High Detail Terrain: 50, Radial Flora: 256, Non-Collidable Flora: 128";

					if (camera)
						camera->setViewDistance(4096);

					TerrainObject::setLevelOfDetailThreshold(16);
					
					TerrainObject::setHighLevelOfDetailThreshold(50);
					ClientProceduralTerrainAppearance::setDynamicNearFloraDistance(256);
					ClientProceduralTerrainAppearance::setStaticNonCollidableFloraDistance(128);
				} else
					resultUnicode += L"No option specified. Syntax is /setall <default|low|medium|high|ultra> or /overrideall <default|low|medium|high|ultra";

				handled = true;
			}

			// Check for other commands
		} else if (afterSlash.size() > 0) {
			auto &command = afterSlash;

			if (command == L"reloadTerrain") {
				if (groundScene)
					groundScene->reloadTerrain();
				else
					resultUnicode = "groundScene is null";

				return true;
			} else if (command == L"assist2") {
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
			} if (command == L"console") {
				static bool installed = false;

				if (!installed) {
					typedef CuiMediatorFactory::Constructor<SwgCuiConsole> ctor_t;

					auto ctor = new ctor_t(("/Console"));

					CuiMediatorFactory::addConstructor("Console", ctor);

					installed = true;

					CuiMediator* console = CuiMediatorFactory::get("Console");

					if (console != nullptr) {
						resultUnicode += L"activating console";

						CuiMediatorFactory::activate("Console");

						consoleActive = true;
					} else {
						resultUnicode += "could not find console in cui mediator factory";
					}
				} else {
					CuiMediatorFactory::toggle("Console");

					consoleActive = !consoleActive;
				}

				return true;
			} if (command == L"testhooks") {
				CreatureObject* creature = Game::getPlayerCreature();
				soe::string debugInfo = creature->getDebugInformation();
				soe::unicode attrInfo;
				soe::string templateName = creature->getObjectTemplateName();

				int checkController = creature == creature->getController()->getOwner();

				ObjectAttributeManager::formatDebugInfo(creature, attrInfo);

				PlayerObject* playerObject = Game::getPlayerObject();
				soe::string strval = "testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing""testing";
				soe::unicode unicodeTest = "testingUnicode";
				int mdasize = strval.size();
				int foundTestInVector = 0;
				auto position = creature->getPosition_p();

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

				if (creature) {
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
					constexpr uint32_t tryConstantTestValue = soe::string::hashCode("bla");

					char message[1024];
					sprintf_s(message, sizeof(message), "Your current health is: %d %s %s %s %d %d %lld %lld %d target: %lld %d %d %d %f %d\n position: %f %f %f ctrl==creo:%d ",
						healthValue, strval.c_str(), str2val.c_str(), testingVector.at(0).c_str(), val1, val2, creoOID.get(), playOID,
						checkVal, targetOID.get(), atts.size(), mdasize, foundTestInVector, 
						ClientProceduralTerrainAppearance::getStaticNonCollidableFloraDistance(), tryConstantTestValue, position.getX(), position.getY(), position.getZ(), checkController);

					resultUnicode += message;
				} else {
					resultUnicode += L"Main Player CreatureObject is null";
				}

				resultUnicode += debugInfo.c_str();

				resultUnicode += attrInfo;
				resultUnicode += templateName.c_str();

				handled = true;
			}
			if (command == L"getcurrenthealth") {
				CreatureObject* creature = Game::getPlayerCreature();

				if (creature) {
					int healthValue = creature->getAttribute(CreatureObject::Health);

					char message[1024];
					sprintf_s(message, sizeof(message), "Your current health is: %d",
						healthValue);

					resultUnicode += message;
				}  else {
					resultUnicode += L"Main Player CreatureObject is null";
				}

				handled = true;
			} else if (command == L"getradialflora") {
				double radialDist = ClientProceduralTerrainAppearance::getDynamicNearFloraDistance();
				auto radialDistString = std::to_string(radialDist);

				resultUnicode += L"Radial Flora Distance: ";
				resultUnicode += radialDistString.c_str();

				handled = true;
			} else if (command == L"getnoncollidableflora" || command == L"getncflora") {
				double ncDist = ClientProceduralTerrainAppearance::getStaticNonCollidableFloraDistance();
				auto ncDistString = std::to_string(ncDist);

				resultUnicode += L"Non-Collidable Flora Distance: ";
				resultUnicode += ncDistString.c_str();

				handled = true;
			} else if (command == L"getviewdistance" || command == L"getvd") {
				if (camera) {
					auto viewDistString = std::to_string(camera->getViewDistance());

					resultUnicode += "View/Rendering Distance: ";
					resultUnicode += viewDistString.c_str();
				}

				handled = true;
			} else if (command == L"exthelp") {
				resultUnicode += L"[COMMAND HELP] Settings Override Extensions by N00854180T\n";
				resultUnicode += L"/globaldetail X - sets the Global Detail Level to X, valid ranges are 1-24. You must move the slider in Terrain options AFTER using this command.\n";
				resultUnicode += L"<hdterrain|/highdetailterrain> X - sets the High Detail Terrain Distance to X, valid ranges are 1-50. You must move the slider in Terrain options AFTER using this command.\n";
				resultUnicode += L"/radialflora X - sets the Radial Flora Distance to X, valid ranges are 1-256.\n";
				resultUnicode += L"</ncflora|/noncollidableflora> X - sets the Non-Collidable Flora Distance to X, valid ranges are 1-128.\n";
				resultUnicode += L"</vd|/viewdistance> X - sets the View/Rendering Distance to X, valid ranges are 512-4096.\n";
				resultUnicode += L"</getvd|/getviewdistance> - Prints the current View/Rendering Distance value to the chat.\n";
				resultUnicode += L"/getradialflora - Prints the current Radial Flora Distance value to the chat.\n";
				resultUnicode += L"</getncflora|/getnoncollidableflora> - Prints the current Non-Collidable Flora Distance value to the chat.\n";
				resultUnicode += L"</setall|/overrideall> <default|low|medium|high|ultra> - Sets all graphics settings to preset values. Type /overrideall help for info on each preset.\n";
				resultUnicode += L"/exthelp - This command, which lists help info on available extension commands.\n";

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