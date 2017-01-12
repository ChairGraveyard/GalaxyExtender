#pragma once

#include "Object.h"
#include "soewrappers.h"

#define GAMELANGUAGEMANAGER_GETLANGUAGESPEAKSKILLMODNAME_ADDRESS 0x011C6270

class GameLanguageManager {
public:
	static void getLanguageSpeakSkillModName(int languageID, soe::string& skillModName) {
		Call<GAMELANGUAGEMANAGER_GETLANGUAGESPEAKSKILLMODNAME_ADDRESS, void, int, soe::string&>::run(languageID, skillModName);
	}
};