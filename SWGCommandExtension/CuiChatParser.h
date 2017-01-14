#pragma once

#include "soewrappers.h"
#include "Object.h"

// Technically this isn't the command handler, it's just the handler
// for language commands, moods, etc., but it's perfect for our case as 
// it's the last type of command checked, which means we know most others
// have gotten a chance to be processed before us.
#define COMMAND_HANDLER_ADDRESS 0x9FF6F0

class CuiChatParser {
public:
	static bool parse(const soe::unicode& command, soe::unicode& result, uint32_t chatRoomID, bool useChatRoom);

	DEFINE_HOOOK(COMMAND_HANDLER_ADDRESS, parse, originalParse);
};
