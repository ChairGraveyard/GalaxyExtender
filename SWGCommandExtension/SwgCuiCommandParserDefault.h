#pragma once

#include "Object.h"
#include "soewrappers.h"
#include "CommandParser.h"

#define CMDPARSERDEFAULTCTOR 0x01010980
#define SETALIASSTATIC 0x1010E20
#define REMOVEALIASTSTATIC 0x010114A0

class CommandParserHistory;

class SwgCuiCommandParserDefault : public CommandParser {
	char data[0x58];
public:
	/*SwgCuiCommandParserDefault* ctorHook(void* arg1) {
		SwgCuiCommandParserDefault* ret = originalCtor::run(this, arg1);

		return ret;
	}

	DEFINE_HOOK(CMDPARSERDEFAULTCTOR, ctorHook, originalCtor);*/

	void ctor(CommandParserHistory* history) {
		runMethod<CMDPARSERDEFAULTCTOR, void>(history);
	}

	static bool setAliasStatic(const soe::unicode& key, const soe::unicode& value) {
		return Call<SETALIASSTATIC, bool, const soe::unicode&, const soe::unicode&>::run(key, value);
	}

	static bool removeAliasStatic(const soe::unicode& key) {
		return Call<REMOVEALIASTSTATIC, bool, const soe::unicode&>::run(key);
	}

	//DEFINE_HOOK(REMOVEALIASTSTATIC, removeAliasStatic, originalRemoveAliasStatic);

	
};