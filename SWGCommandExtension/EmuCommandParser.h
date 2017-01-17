#pragma once

#include "CommandParser.h"
#include "soewrappers.h"
#include "NetworkId.h"

class EmuCommandParser : public CommandParser {
	char data[0x58];

	DEFINE_VTABLE(newVtable);

public:
	void ctor();
	void initializeVtable();

	bool performParsing(const NetworkId& userId, const soe::vector<soe::unicode>& argv,
		const soe::unicode& originalCommand, soe::unicode& result, const CommandParser* node);
	DEFINE_VMETHOD(0x4, performParsing);

	static void showHelp(soe::unicode& resultUnicode);
};
