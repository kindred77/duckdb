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
		PgroupInsert(nullptr /*pgroupTarget*/, expression_ptr, XForm::ExfInvalid,
					 nullptr /*pgexprOrigin*/, false /*fIntermediate*/);
	m_pmemo->SetRoot(group_root);
}

GroupPtr Engine::PgroupInsert(GroupPtr pgroupTarget, ExpressionPtr pexpr,
					  XForm::EXformId exfidOrigin,
					  GroupExpressionPtr pgexprOrigin, bool fIntermediate) {
    GroupPtr pgroupOrigin = nullptr;

    if (nullptr != pexpr->Pgexpr()) {
		pgroupOrigin = pexpr->Pgexpr()->Group();
		//GPOS_ASSERT(nullptr != pgroupOrigin && nullptr == group_target &&
		//			"A valid group is expected");

		// if parent has group pointer, all children must have group pointers;
		// terminate recursive insertion here
		return pgroupOrigin;
	}

	// if we have a valid origin group, target group must be NULL
	//GPOS_ASSERT_IMP(nullptr != pgroupOrigin, nullptr == pgroupTarget);

	// insert expression's children to memo by recursive call
	std::shared_ptr<Groups> pdrgpgroupChildren =
		std::make_shared<Groups>(pexpr->Arity());

	InsertExpressionChildren(pexpr, pdrgpgroupChildren, exfidOrigin,
							 pgexprOrigin);
	
	LogicalOperatorPtr op = pexpr->op();
	GroupExpressionPtr pgexpr = std::make_shared<GroupExpression>(
		op, pdrgpgroupChildren, exfidOrigin,
		pgexprOrigin, fIntermediate);

	// find the group that contains created group expression
	GroupPtr pgroupContainer =
		m_pmemo->GroupInsert(pgroupTarget, pexpr, pgexpr);

	if (nullptr == pgexpr->Group())
	{
		// insertion failed, release created group expression
		pgexpr->Release();
	}

	return pgroupContainer;
}

void
Engine::InsertExpressionChildren(ExpressionPtr expr,
		std::shared_ptr<Groups> pdrgpgroupChildren,
		XForm::EXformId exfidOrigin,
		GroupExpressionPtr pgexprOrigin) {
	size_t arity = expr->Arity();

	for (auto i = 0; i < arity; i++) {
		GroupPtr pgroupChild = nullptr;
		LogicalOperatorPtr popChild = expr->operator[](i)->op();
		if (popChild->FPattern() && CPattern::PopConvert(popChild)->FLeaf()) {
			//GPOS_ASSERT(nullptr != (*pexpr)[i]->Pgexpr()->Pgroup());

			// group is already assigned during binding extraction;
			pgroupChild = expr->operator[](i)->GroupExpression()->Group();
		} else {
			// insert child expression recursively
			pgroupChild =
				PgroupInsert(nullptr /*pgroupTarget*/, expr->operator[](i), exfidOrigin,
							 pgexprOrigin, true /*fIntermediate*/);
		}
		pdrgpgroupChildren->push_back(pgroupChild);
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
