#pragma once

#include "CuiConsoleHelperParserStrategy.h"


class CommandParser;

class CuiChatParserStrategy : public CuiConsoleHelperParserStrategy {
	char data[0x1C];

public:
	void ctor(CommandParser* parser, bool global) {
		runMethod<0x013DE6E0, void>(parser, global);
	}
};