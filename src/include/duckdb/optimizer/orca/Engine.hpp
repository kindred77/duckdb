#pragma once

#include "duckdb/optimizer/orca/SearchStage.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/optimizer/orca/QueryContext.hpp"
#include "duckdb/optimizer/orca/search/Group.hpp"
#include "duckdb/optimizer/orca/search/GroupExpression.hpp"
#include "duckdb/optimizer/orca/XForm.hpp"
#include "duckdb/optimizer/orca/search/Memo.hpp"
#include "duckdb/optimizer/orca/SearchStage.hpp"

namespace duckdb {

class QueryContext;

using LogicalOperatorPtr = std::shared_ptr<LogicalOperator>;

class Engine;
using EnginePtr = std::shared_ptr<Engine>;

class Engine : std::enable_shared_from_this<Engine> {
private:
    QueryContextPtr m_pqc;
    std::shared_ptr<SearchStages> m_search_stage_array;
    // index of current search stage
	size_t m_ulCurrSearchStage;

    MemoPtr m_pmemo;

	// set of activated xforms
	XFormSetPtr m_xforms;

    bool
	FSearchTerminated() const
	{
		// at least one stage has completed and achieved required cost
		return (nullptr != PssPrevious() && PssPrevious()->FAchievedReqdCost());
	}

    // create and schedule the main optimization job
	void ScheduleMainJob(SchedulerContextPtr psc,
						 OptimizationContextPtr poc) const;
public:
    Engine(const QueryContext & query_context, const SearchStages & search_stages);
    void InitLogicalExpression(ExpressionPtr expression_ptr);
    GroupPtr GroupInsert(GroupPtr group_target, ExpressionPtr expression_ptr,
					  XForm::EXformId xformid_origin,
					  GroupExpressionPtr group_exp_origin, bool fIntermediate);
    void
    InsertExpressionChildren(ExpressionPtr expr,
								  std::shared_ptr<Groups> group_children,
								  XForm::EXformId exfid_origin,
								  GroupExpressionPtr group_expr_origin);
    
    // accessor of memo's root group
	GroupPtr
	PgroupRoot() const
	{
		//GPOS_ASSERT(nullptr != m_pmemo);

		return m_pmemo->PgroupRoot();
	}

    // return current search stage
	SearchStagePtr
	PssCurrent() const
	{
		return m_search_stage_array->operator[](m_ulCurrSearchStage);
	}

	// current search stage index accessor
	uint32_t
	UlCurrSearchStage() const
	{
		return m_ulCurrSearchStage;
	}

    // return previous search stage
	SearchStagePtr
	PssPrevious() const {
		if (0 == m_ulCurrSearchStage) {
			return nullptr;
		}

		return m_search_stage_array->operator[](m_ulCurrSearchStage - 1);
	}

	void FinalizeSearchStage();

    void Optimize();
};

}