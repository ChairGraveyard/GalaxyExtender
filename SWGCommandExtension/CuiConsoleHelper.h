#pragma once

#include "stlwrappers.h"
#include <vector>

class CuiConsoleHelperParserStrategy
{
public:
	virtual bool parse(const stlportunicode* str, stlportunicode* result) const = 0;
	virtual bool tabCompleteToken(const stlportunicode* str, size_t tokenPos, 
		std::vector<stlportunicode>::fwd & results, size_t & token_start, size_t & token_end) const = 0;
	virtual ~CuiConsoleHelperParserStrategy() = 0;
};