#pragma once

#include "CommandParser.h"
#include "soewrappers.h"
#include "NetworkId.h"

class EmuCommandParser : public CommandParser {
	char data[0x58];

	static uint32_t newVtable[];
public:
	void ctor();

	bool performParsing(const NetworkId& userId, const soe::vector<soe::unicode>& argv,
		const soe::unicode& originalCommand, soe::unicode& result, const CommandParser* node);

	DEFINE_HOOK(124235346, performParsing, originalPerformParsing);

	static void showHelp(soe::unicode& resultUnicode);
};
