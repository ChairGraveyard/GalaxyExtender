#include "stdafx.h"

#include "CommandParser.h"

uint32_t CommandParser::newVtable[] = { 0x161E6CC, 0x15EA3C4, 0x15EA3C8 };

void CommandParser::ctor(const char* cmd, size_t min, const char* args, const char* help, CommandParser* arg5) {
	runMethod<0xA83EF0, void>(cmd, min, args, help, arg5);

	SETVTABLE(newVtable);
}