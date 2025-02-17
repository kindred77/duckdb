#pragma once

#include "duckdb/optimizer/orca/search/GroupExpression.hpp"
#include "duckdb/optimizer/orca/search/OptimizationContext.hpp"
#include "duckdb/optimizer/orca/statistics/IStatistics.hpp"

// infinite plan cost
#define GPOPT_INFINITE_COST CCost(1e+100)

// invalid cost value
#define GPOPT_INVALID_COST CCost(-0.5)

namespace duckdb {

class CostContext {
public:
	// states of cost context
	enum EState {
		estUncosted,  // initial state

		estCosting,	 // ongoing costing
		estCosted,	 // done costing

		estSentinel
	};
private:
    // cost of group expression under optimization context
	Cost m_cost;

	// cost context state
	EState m_estate;

	// back pointer to owner group expression
	GroupExpressionPtr m_pgexpr;

	// group expression to be used stats derivation during costing
	GroupExpressioPtr m_pgexprForStats;

	// array of optimization contexts of child groups
	COptimizationContextArray *m_pdrgpoc;

	// derived properties of the carried plan
	DrvdPropPlanPtr m_pdpplan;

	// optimization request number
	uint32_t m_ulOptReq;

	// flag to indicate if cost context is pruned,
	// a cost context is pruned during branch-and-bound search if there exists
	// an equivalent context with better cost
	bool m_fPruned;

	// stats of owner group expression
	IStatisticsPtr m_pstats;

	// derive stats of owner group expression
	void DeriveStats();

	// return the number of rows per host
	double DRowsPerHost() const;

	// for two cost contexts with join plans of the same cost, break the tie based on join depth,
	// if tie-resolution succeeded, store a pointer to preferred cost context in output argument
	static void BreakCostTiesForJoinPlans(const CCostContext *pccFst,
										  const CCostContext *pccSnd,
										  CONST_COSTCTXT_PTR *ppccPrefered,
										  BOOL *pfTiesResolved);
};

}
