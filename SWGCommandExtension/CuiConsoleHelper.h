#pragma once

#include "soewrappers.h"

class CuiConsoleHelperParserStrategy {
public:
	virtual bool parse(const soe::unicode& str, soe::unicode& result) const = 0;

	virtual bool tabCompleteToken(const soe::unicode* str, size_t tokenPos, 
		soe::vector<soe::unicode>& results, size_t & token_start, size_t & token_end) const = 0;

	virtual ~CuiConsoleHelperParserStrategy() = 0;
};