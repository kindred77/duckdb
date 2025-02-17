#pragma once

#include "duckdb/optimizer/orca/Group.hpp"

#define GPOPT_INVALID_OPTCTXT_ID gpos::ulong_max

namespace duckdb {

class OptimizationContext;
using OptimizationContextPtr = std::shared_ptr<OptimizationContext>;

class OptimizationContext {
private:
    // private copy ctor
	OptimizationContext(const OptimizationContext &);

	// unique id within owner group, used for debugging
	uint32_t m_id{GPOPT_INVALID_OPTCTXT_ID};

	// back pointer to owner group, used for debugging
	GroupPtr m_pgroup{nullptr};
public:
	// states of optimization context
	enum EState {
		estUnoptimized,	 // initial state

		estOptimizing,	// ongoing optimization
		estOptimized,	// done optimization

		estSentinel
	};

	// ctor
	OptimizationContext(
		GroupPtr pgroup, CReqdPropPlan *prpp,
		CReqdPropRelational *
			prprel,	 // required relational props -- used during stats derivation
		IStatisticsArray
			*stats_ctxt,  // stats of previously optimized expressions
		uint32_t ulSearchStageIndex)
		: m_pgroup(pgroup),
		  m_prpp(prpp),
		  m_prprel(prprel),
		  m_pdrgpstatCtxt(stats_ctxt),
		  m_ulSearchStageIndex(ulSearchStageIndex) {
		// GPOS_ASSERT(nullptr != pgroup);
		// GPOS_ASSERT(nullptr != prpp);
		// GPOS_ASSERT(nullptr != prprel);
		// GPOS_ASSERT(nullptr != stats_ctxt);
	}

	// dtor
	~OptimizationContext();

	// best group expression accessor
	GroupExpressionPtr PgexprBest() const;

	// match optimization contexts
	bool Matches(const OptimizationContextPtr poc) const;

	// get id
	uint32_t
	Id() const {
		return m_id;
	}
};

}
