#pragma once

#include "duckdb/optimizer/orca/base/OptimizationContext.hpp"

namespace duckdb {

// optimization levels in ascending order,
// under a given optimization context, group expressions in higher levels
// must be optimized before group expressions in lower levels,
// a group expression sets its level in CGroupExpression::SetOptimizationLevel()
enum EOptimizationLevel
{
	EolLow = 0,	 // low optimization level, this is the default level
	EolHigh,	 // high optimization level

	EolSentinel
};

class Group;
using GroupPtr = std::shared_ptr<Group>;

using Groups = std::vector<GroupPtr>;

class Group {
public:
    Group(bool fScalar = false);

    // insert given context into contexts hash table
	OptimizationContextPtr PocInsert(OptimizationContextPtr poc);
};

}