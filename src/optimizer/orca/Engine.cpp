#include "duckdb/optimizer/orca/Engine.hpp"
#include "duckdb/optimizer/orca/XForm.hpp"
#include "duckdb/optimizer/orca/Expression.hpp"
#include "duckdb/optimizer/orca/search/JobFactory.hpp"
#include "duckdb/optimizer/orca/search/Scheduler.hpp"
#include "duckdb/optimizer/orca/search/SchedulerContext.hpp"
#include "duckdb/optimizer/orca/search/JobGroupOptimization.hpp"
#include "duckdb/optimizer/orca/base/OptimizationContext.hpp"
#include "duckdb/optimizer/orca/base/ReqdPropRelational.hpp"
#include "duckdb/optimizer/orca/base/ColRefSet.hpp"
#include "duckdb/optimizer/orca/statistics/Statistics.hpp"


#define GPOPT_SAMPLING_MAX_ITERS 30
#define GPOPT_JOBS_CAP 5000	 // maximum number of initial optimization jobs
#define GPOPT_JOBS_PER_GROUP \
	20	// estimated number of needed optimization jobs per memo group

// memory consumption unit in bytes -- currently MB
#define GPOPT_MEM_UNIT (1024 * 1024)
#define GPOPT_MEM_UNIT_NAME "MB"

namespace duckdb {

Engine::Engine(const QueryContext & query_context, const SearchStages & search_stages)
    : query_context(query_context), m_search_stage_array(search_stages) {
    if (m_search_stage_array.empty) {
		m_search_stage_array = SearchStage::GenderateDefault();
	}
}

void Engine::InitLogicalExpression(ExpressionPtr expression_ptr) {
    GroupPtr group_root =
		GroupInsert(nullptr /*pgroupTarget*/, expression_ptr, XForm::ExfInvalid,
					 nullptr /*pgexprOrigin*/, false /*fIntermediate*/);
	m_pmemo->SetRoot(group_root);
}

GroupPtr Engine::GroupInsert(GroupPtr group_target, ExpressionPtr expression_ptr,
					  XForm::EXformId xformid_origin,
					  GroupExpressionPtr group_exp_origin, bool fIntermediate) {
    GroupPtr group_origin = nullptr;

    if (nullptr != expression_ptr->GroupExpression()) {
		group_origin = expression_ptr->GroupExpression()->Group();
		//GPOS_ASSERT(nullptr != group_origin && nullptr == group_target &&
		//			"A valid group is expected");

		// if parent has group pointer, all children must have group pointers;
		// terminate recursive insertion here
		return group_origin;
	}

	// if we have a valid origin group, target group must be NULL
	//GPOS_ASSERT_IMP(nullptr != pgroupOrigin, nullptr == pgroupTarget);

	// insert expression's children to memo by recursive call
	std::shared_ptr<Groups> group_children =
		std::make_shared<Groups>(expression_ptr->Arity());

	InsertExpressionChildren(expression_ptr, group_children, xformid_origin,
							 group_exp_origin);
	
	LogicalOperatorPtr op = expression_ptr->op();
	GroupExpressionPtr group_expr = std::make_shared<GroupExpression>(
		op, group_children, xformid_origin,
		group_exp_origin, fIntermediate);

	// find the group that contains created group expression
	GroupPtr group_container =
		m_pmemo->GroupInsert(group_target, expression_ptr, group_expr);

	if (nullptr == group_expr->Group())
	{
		// insertion failed, release created group expression
		group_expr->Release();
	}

	return group_container;
}

void
Engine::InsertExpressionChildren(ExpressionPtr expr,
		std::shared_ptr<Groups> group_children,
		XForm::EXformId exfid_origin,
		GroupExpressionPtr group_expr_origin) {
	size_t arity = expr->Arity();

	for (auto i = 0; i < arity; i++) {
		GroupPtr group_child = nullptr;
		LogicalOperatorPtr op_child = expr->operator[](i)->op();
		if (op_child->FPattern() && CPattern::PopConvert(op_child)->FLeaf()) {
			//GPOS_ASSERT(nullptr != (*pexpr)[i]->Pgexpr()->Pgroup());

			// group is already assigned during binding extraction;
			group_child = expr->operator[](i)->GroupExpression()->Group();
		} else {
			// insert child expression recursively
			group_child =
				GroupInsert(nullptr /*pgroupTarget*/, expr->operator[](i), exfid_origin,
							 group_expr_origin, true /*fIntermediate*/);
		}
		group_children->push_back(group_child);
	}
}

void
Engine::ScheduleMainJob(SchedulerContextPtr psc,
						 OptimizationContextPtr poc) const {
	//GPOS_ASSERT(nullptr != PgroupRoot());

	JobGroupOptimization::ScheduleJob(
		psc, PgroupRoot(), nullptr /*pgexprOrigin*/, poc, nullptr /*pjParent*/);
}

void
Engine::FinalizeSearchStage()
{
	//ProcessTraceFlags();

	m_xforms->Release();
	m_xforms = nullptr;
	m_xforms = std::make_shared<XFormSet>();

	m_ulCurrSearchStage++;
	m_pmemo->ResetGroupStates();
}

void
Engine::Optimize() {
	const size_t ulJobs =
		std::min(GPOPT_JOBS_CAP, m_pmemo->UlpGroups() * GPOPT_JOBS_PER_GROUP);
	JobFactoryPtr jf = std::make_shared<JobFactory>(ulJobs);
	SchedulerPtr sched = std::make_shared<Scheduler>(ulJobs);

	SchedulerContext sc;
	sc.Init(jf, sched, std::shared_from_this());

	const uint32_t ulSearchStages = m_search_stage_array->size();
	for (size_t ul = 0; !FSearchTerminated() && ul < ulSearchStages; ul++) {
		PssCurrent()->RestartTimer();
		
		OptimizationContextPtr poc = std::make_shared<OptimizationContext>(
			PgroupRoot(), m_pqc->Prpp(),
			std::make_shared<ReqdPropRelational>(std::make_shared<ColRefSet>()),
			std::make_shared<IStatisticsArray>(),
			m_ulCurrSearchStage
		);

		// schedule main optimization job
		ScheduleMainJob(&sc, poc);

		// run optimization job
		Scheduler::Run(&sc);

		// extract best plan found at the end of current search stage
		ExpressionPtr pexprPlan = m_pmemo->PexprExtractPlan(
			m_pmemo->PgroupRoot(), m_pqc->Prpp(),
			m_search_stage_array->size());
		PssCurrent()->SetBestExpr(pexprPlan);

		FinalizeSearchStage();
	}
}

}
