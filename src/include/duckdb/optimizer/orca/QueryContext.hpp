#pragma once

#include "duckdb/optimizer/orca/base/ReqdPropPlan.hpp"

namespace duckdb {

class QueryContext;
using QueryContextPtr = std::shared_ptr<QueryContext>;

class QueryContext {
private:
	// required plan properties in optimizer's produced plan
	ReqdPropPlanPtr m_prpp;
public:
    // required plan properties accessor
	ReqdPropPlanPtr
	Prpp() const
	{
		return m_prpp;
	}
};

}
