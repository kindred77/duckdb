#include "duckdb/optimizer/orca/search/JobGroupOptimization.hpp"

namespace duckdb {

const JobGroupOptimization::EEvent
	rgeev[JobGroupOptimization::estSentinel]
		 [JobGroupOptimization::estSentinel] = {
			 {// estInitialized
			  JobGroupOptimization::eevImplementing,
			  JobGroupOptimization::eevImplemented,
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevOptimized},
			 {// estOptimizingChildren
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevOptimizing,
			  JobGroupOptimization::eevOptimizedCurrentLevel,
			  JobGroupOptimization::eevSentinel},
			 {// estDampingOptimizationLevel
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevOptimizing,
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevOptimized},
			 {// estCompleted
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevSentinel,
			  JobGroupOptimization::eevSentinel},
};

void JobGroupOptimization::Init(GroupPtr pgroup, GroupExpressionPtr pgexprOrigin,
			  OptimizationContextPtr poc) {
    //GPOS_ASSERT(nullptr != poc);
	//GPOS_ASSERT(pgroup == poc->Pgroup());

	JobGroup::Init(pgroup);
	m_jsm.Init(rgeev
#ifdef GPOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// GPOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtStartOptimization);
	m_jsm.SetAction(estOptimizingChildren, EevtOptimizeChildren);
	m_jsm.SetAction(estDampingOptimizationLevel, EevtCompleteOptimization);

	m_pgexprOrigin = pgexprOrigin;
	m_poc = m_pgroup->PocInsert(poc);
	if (poc == m_poc)
	{
		// pin down context in hash table
		m_poc->AddRef();
	}
	SetJobQueue(m_poc->PjqOptimization());

	// initialize current optimization level as low
	m_eolCurrent = EolLow;

	Job::SetInit();
}

void JobGroupOptimization::ScheduleJob(SchedulerContextPtr psc, GroupPtr pgroup,
							GroupExpressionPtr pgexprOrigin,
							OptimizationContextPtr poc, JobPtr pjParent) {
    JobPtr pj = psc->Pjf()->PjCreate(Job::EjtGroupOptimization);

	// initialize job
	JobGroupOptimizationPtr pjgo = PjConvert(pj);
	pjgo->Init(pgroup, pgexprOrigin, poc);
	psc->Psched()->Add(pjgo, pjParent);
}

}
