#pragma once
#include <cstdint>

class NetworkId {
public:
	uint64_t objectID = 0;

	NetworkId() = default;
	NetworkId(const NetworkId&) = default;
	NetworkId(NetworkId&&) = default;
	NetworkId(uint64_t oid) : objectID(oid) {
	}

	static NetworkId empty;

	const uint64_t& get() const {
		return objectID;
	}

	uint64_t& get() {
		return objectID;
	}

	operator uint64_t() const {
		return objectID;
	}

	operator uint64_t&()  {
		return objectID;
	}
};
