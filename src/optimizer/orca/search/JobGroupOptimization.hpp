#pragma once

#include "duckdb/optimizer/orca/base/OptimizationContext.hpp"
#include "duckdb/optimizer/orca/search/JobGroup.hpp"
#include "duckdb/optimizer/orca/search/SchedulerContext.hpp"
#include "duckdb/optimizer/orca/search/Group.hpp"
#include "duckdb/optimizer/orca/search/GroupExpression.hpp"
#include "duckdb/optimizer/orca/search/Job.hpp"
#include "duckdb/optimizer/orca/search/JobStateMachine.hpp"

namespace duckdb {

class JobGroupOptimization;
using JobGroupOptimizationPtr = std::shared_ptr<JobGroupOptimization>;

class JobGroupOptimization : public JobGroup {

private:
	// shorthand for job state machine
	using JSM = JobStateMachine<EState, estSentinel, EEvent, eevSentinel>;

	// job state machine
	JSM m_jsm;

	// group expression that triggered group optimization
	GroupExpressionPtr m_pgexprOrigin;

	// optimization context of the job
	OptimizationContextPtr m_poc;

	// current optimization level of group expressions
	EOptimizationLevel m_eolCurrent;

	// start optimization action
	static EEvent EevtStartOptimization(SchedulerContextPtr psc, JobPtr pj);

	// optimized child group expressions action
	static EEvent EevtOptimizeChildren(SchedulerContextPtr psc, JobPtr pj);

	// complete optimization action
	static EEvent EevtCompleteOptimization(SchedulerContextPtr psc, JobPtr pj);

public:
	// transition events of group optimization
	enum EEvent {
		eevImplementing,		   // implementation is in progress
		eevImplemented,			   // implementation is complete
		eevOptimizing,			   // optimization is in progress
		eevOptimizedCurrentLevel,  // optimization of current level is complete
		eevOptimized,			   // optimization is complete

		eevSentinel
	};

	// states of group optimization job
	enum EState {
		estInitialized = 0,			  // initial state
		estOptimizingChildren,		  // optimizing group expressions
		estDampingOptimizationLevel,  // damping optimization level
		estCompleted,				  // done optimization

		estSentinel
	};

	// initialize job
	void Init(GroupPtr pgroup, GroupExpressionPtr pgexprOrigin,
			  OptimizationContextPtr poc);

    // schedule a new group optimization job
	static void ScheduleJob(SchedulerContextPtr psc, GroupPtr pgroup,
							GroupExpressionPtr pgexprOrigin,
							OptimizationContextPtr poc, JobPtr pjParent);
	
	// conversion function
	static JobGroupOptimizationPtr
	PjConvert(JobPtr pj) {
		//GPOS_ASSERT(nullptr != pj);
		//GPOS_ASSERT(EjtGroupOptimization == pj->Ejt());

		return dynamic_cast<JobGroupOptimizationPtr>(pj);
	}
};

}
