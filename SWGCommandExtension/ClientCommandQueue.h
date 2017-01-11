#pragma once

#include "NetworkId.h"
#include "soewrappers.h"

#define CLIENTCOMMANDQUEUE_ENQUEUECOMMAND_ADDRESS 0x46E5F0

class ClientCommandQueue {
public:
	static void enqueueCommand(uint32_t commandHashCode, const NetworkId& target, const soe::unicode& parameters) {
		Call<CLIENTCOMMANDQUEUE_ENQUEUECOMMAND_ADDRESS,  /*func address*/
			void, /*return type*/
			uint32_t, const NetworkId&, const soe::unicode&>::run(commandHashCode, target, parameters);
	}

	static void enqueueCommand(uint32_t commandHashCode, const NetworkId& target) {
		enqueueCommand(commandHashCode, target, soe::unicode::empty_string());
	}

	static void enqueueCommand(uint32_t commandHashCode) {
		enqueueCommand(commandHashCode, NetworkId::empty, soe::unicode::empty_string());
	}


};