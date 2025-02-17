#include "duckdb/optimizer/orca/search/Memo.hpp"
#include "duckdb/optimizer/orca/base/OptimizationContext.hpp"
#include "duckdb/optimizer/orca/base/CostContext.hpp"
#include "duckdb/optimizer/orca/cost/Cost.hpp"
#include "duckdb/optimizer/orca/statistics/IStatistics.hpp"

namespace duckdb {

Memo::Memo() {

}

void Memo::SetRoot(GroupPtr group_ptr) {
    m_pgroupRoot = group_ptr;
}

GroupPtr Memo::PgroupInsert(GroupPtr group_target, ExpressionPtr expr_origin,
					GroupExpressionPtr group_expr) {
    GroupPtr group_container = nullptr;
	GroupExpressionPtr group_expr_found = nullptr;
	// hash table accessor's scope
	{
		ShtAcc shta(m_sht, *pgexpr);
		pgexprFound = shta.Find();
	}

    bool fNewGroup =
		FNewGroup(&pgroupTarget, group_expr_found, group_expr->op()->FScalar());
	if (fNewGroup) {
		// we may add a new group to Memo, so we derive props here
		(void) expr_origin->PdpDerive();
	}

	if (nullptr != group_expr_found) {
		group_container = group_expr_found->Group();
	} else {
		group_container =
			PgroupInsert(group_target, group_expr, expr_origin, fNewGroup);
	}

	// if insertion failed, release group as needed
	if (nullptr == group_expr->Group() && fNewGroup) {
		fNewGroup = false;
		group_target->Release();
	}

	// if a new scalar group is added, we materialize a scalar expression
	// for statistics derivation purposes
	if (fNewGroup && group_target->FScalar()) {
		group_target->CreateScalarExpression();
		group_target->CreateDummyCostContext();
	}

	return group_container;
}

ExpressionPtr Memo::PexprExtractPlan(GroupPtr pgroupRoot,
								  ReqdPropPlanPtr prppInput,
								  uint32_t ulSearchStages) {
	// check stack size
	//GPOS_CHECK_STACK_SIZE;
	//GPOS_CHECK_ABORT;

	GroupExpressionPtr pgexprBest = nullptr;
	OptimizationContextPtr poc = nullptr;
	Cost cost = GPOPT_INVALID_COST;
	IStatisticsPtr stats = nullptr;

	
}

void
Memo::ResetGroupStates()
{
	GroupPtr pgroup = m_listGroups.PtFirst();

	while (nullptr != pgroup)
	{
		pgroup->ResetGroupState();
		pgroup->ResetGroupJobQueues();
		pgroup->ResetHasNewLogicalOperators();

		pgroup = m_listGroups.Next(pgroup);
	}
}

}
