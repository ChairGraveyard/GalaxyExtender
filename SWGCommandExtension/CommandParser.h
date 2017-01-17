#pragma once

#include "Object.h"

class CommandParser : public BaseHookedObject {
	static uint32_t newVtable[];

public:
	/*CommandParser(const char* cmd, size_t min, const char* args, const char* help, CommandParser* arg5) {
		uint32_t* currentVtable = *(uint32_t**) this;

		CommandParser::ctor(cmd, min, args, help, arg5);

		//restore to our vtable
		*((uint32_t**) this) = currentVtable;
	}

	virtual ~CommandParser() {
		CommandParser::dtor();
	}

	virtual bool performParsing(const NetworkId& userId, const soe::vector<soe::string>& argv, 
		const soe::string& originalCommand, soe::string& result, const CommandParser* node) {

		return false;
	}*/

	CommandParser* addSubCommand(CommandParser* parser) {
		return runMethod<0x00A85CD0, CommandParser*>(parser);
	}

	void dtor() {
		runMethod<0xA844A0, void>();
	}

	void ctor(const char* cmd, size_t min, const char* args, const char* help, CommandParser* arg5);
};