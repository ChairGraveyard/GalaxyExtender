#pragma once

#include "NetworkId.h"
#include "soewrappers.h"

#define CLIENTCOMMANDQUEUE_ENQUEUECOMMAND_ADDRESS 0x46E5F0

class ClientCommandQueue {
public:
	static void enqueueCommand(uint32_t commandHashCode, const NetworkId& target = NetworkId::empty, const soe::unicode& parameters = soe::unicode::empty_string) {
		Call<CLIENTCOMMANDQUEUE_ENQUEUECOMMAND_ADDRESS,  /*func address*/
			void, /*return type*/
			uint32_t, const NetworkId&, const soe::unicode&>::run(commandHashCode, target, parameters);
	}

};