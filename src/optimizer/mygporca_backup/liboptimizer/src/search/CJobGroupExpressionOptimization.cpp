//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CJobGroupExpressionOptimization.cpp
//
//	@doc:
//		Implementation of group expression optimization job
//---------------------------------------------------------------------------

#include "gpopt/search/CJobGroupExpressionOptimization.h"

#include "gpopt/engine/CEngine.h"
#include "gpopt/search/CGroup.h"
#include "gpopt/search/CGroupExpression.h"
#include "gpopt/search/CJobFactory.h"
#include "gpopt/search/CJobGroupImplementation.h"
#include "gpopt/search/CJobTransformation.h"
#include "gpopt/search/CScheduler.h"
#include "gpopt/search/CSchedulerContext.h"


using namespace gpopt;

// State transition diagram for group expression optimization job state machine;
//
//                 +------------------------+
//                 |    estInitialized:     |
//  +------------- |    EevtInitialize()    |
//  |              +------------------------+
//  |                |
//  |                | eevOptimizingChildren
//  |                v
//  |              +------------------------+   eevOptimizingChildren
//  |              | estOptimizingChildren: | ------------------------+
//  |              | EevtOptimizeChildren() |                         |
//  +------------- |                        | <-----------------------+
//  |              +------------------------+
//  |                |
//  | eevFinalized   | eevChildrenOptimized
//  |                v
//  |              +------------------------+
//  |              | estChildrenOptimized:  |
//  +------------- |   EevtAddEnforcers()   |
//  |              +------------------------+
//  |                |
//  |                | eevOptimizingSelf
//  |                v
//  |              +------------------------+   eevOptimizingSelf
//  |              |  estEnfdPropsChecked:  | ------------------------+
//  |              |   EevtOptimizeSelf()   |                         |
//  +------------- |                        | <-----------------------+
//  |              +------------------------+
//  |                |
//  |                | eevSelfOptimized
//  |                v
//  |              +------------------------+
//  |              |   estSelfOptimized:    |
//  | eevFinalized |     EevtFinalize()     |
//  |              +------------------------+
//  |                |
//  |                |
//  |                |
//  |                |
//  +----------------+
//                   |
//                   |
//                   | eevFinalized
//                   v
//                 +------------------------+
//                 |      estCompleted      |
//                 +------------------------+
//
const CJobGroupExpressionOptimization::EEvent
	rgeev[CJobGroupExpressionOptimization::estSentinel]
		 [CJobGroupExpressionOptimization::estSentinel] = {
			 {// estInitialized
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingChildren,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estOptimizingChildren
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingChildren,
			  CJobGroupExpressionOptimization::eevChildrenOptimized,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estChildrenOptimized
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingSelf,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estEnfdPropsChecked
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevOptimizingSelf,
			  CJobGroupExpressionOptimization::eevSelfOptimized,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estSelfOptimized
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevFinalized},
			 {// estCompleted
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel,
			  CJobGroupExpressionOptimization::eevSentinel},
};

#ifdef GPOS_DEBUG

// names for states
const WCHAR rgwszStates[CJobGroupExpressionOptimization::estSentinel]
					   [GPOPT_FSM_NAME_LENGTH] = {
						   GPOS_WSZ_LIT("initialized"),
						   GPOS_WSZ_LIT("optimizing children"),
						   GPOS_WSZ_LIT("children optimized"),
						   GPOS_WSZ_LIT("enforceable properties checked"),
						   GPOS_WSZ_LIT("self optimized"),
						   GPOS_WSZ_LIT("completed")};

// names for events
const WCHAR rgwszEvents[CJobGroupExpressionOptimization::eevSentinel]
					   [GPOPT_FSM_NAME_LENGTH] = {
						   GPOS_WSZ_LIT("optimizing child groups"),
						   GPOS_WSZ_LIT("optimized child groups"),
						   GPOS_WSZ_LIT("checking enforceable properties"),
						   GPOS_WSZ_LIT("computing group expression cost"),
						   GPOS_WSZ_LIT("computed group expression cost"),
						   GPOS_WSZ_LIT("finalized")};

#endif	// GPOS_DEBUG


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::CJobGroupExpressionOptimization
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::CJobGroupExpressionOptimization()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::~CJobGroupExpressionOptimization
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::~CJobGroupExpressionOptimization()
{
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::Init
//
//	@doc:
//		Initialize job
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::Init(CGroupExpression *pgexpr,
									  COptimizationContext *poc, ULONG ulOptReq,
									  CReqdPropPlan *prppCTEProducer)
{
	GPOS_ASSERT(NULL != poc);

	CJobGroupExpression::Init(pgexpr);
	GPOS_ASSERT(pgexpr->Pop()->FPhysical());
	GPOS_ASSERT(pgexpr->Pgroup() == poc->Pgroup());
	//GPOS_ASSERT(ulOptReq <=
	//			CPhysical::PopConvert(pgexpr->Pop())->UlOptRequests());

	m_jsm.Init(rgeev
#ifdef GPOS_DEBUG
			   ,
			   rgwszStates, rgwszEvents
#endif	// GPOS_DEBUG
	);

	// set job actions
	m_jsm.SetAction(estInitialized, EevtInitialize);
	m_jsm.SetAction(estOptimizingChildren, EevtOptimizeChildren);
	m_jsm.SetAction(estChildrenOptimized, EevtAddEnforcers);
	m_jsm.SetAction(estEnfdPropsChecked, EevtOptimizeSelf);
	m_jsm.SetAction(estSelfOptimized, EevtFinalize);

	m_pdrgpoc = NULL;

	m_ulArity = pgexpr->Arity();
	m_ulChildIndex = gpos::ulong_max;

	m_poc = poc;
	m_ulOptReq = ulOptReq;
	m_fChildOptimizationFailed = false;


	CJob::SetInit();
}

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::Cleanup
//
//	@doc:
//		Cleanup allocated memory
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::Cleanup()
{

}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtInitialize
//
//	@doc:
//		Initialize internal data structures;
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtInitialize(CSchedulerContext *psc,
												CJob *pjOwner)
{

	return eevOptimizingChildren;
}

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::ScheduleChildGroupsJobs
//
//	@doc:
//		Schedule optimization job for the next child group; skip child groups
//		as they do not require optimization
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::ScheduleChildGroupsJobs(CSchedulerContext *psc)
{

}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtOptimizeChildren
//
//	@doc:
//		Optimize child groups
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtOptimizeChildren(CSchedulerContext *psc,
													  CJob *pjOwner)
{
	// get a job pointer
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pjOwner);
	if (0 < pjgeo->m_ulArity && !pjgeo->FChildrenScheduled())
	{
		pjgeo->ScheduleChildGroupsJobs(psc);
		if (pjgeo->m_fChildOptimizationFailed)
		{
			// failed to optimize child, terminate job
			pjgeo->Cleanup();
			return eevFinalized;
		}

		return eevOptimizingChildren;
	}

	return eevChildrenOptimized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtAddEnforcers
//
//	@doc:
//		Add required enforcers to owning group
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtAddEnforcers(CSchedulerContext *psc,
												  CJob *pjOwner)
{

	return eevFinalized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtOptimizeSelf
//
//	@doc:
//		Optimize group expression
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtOptimizeSelf(CSchedulerContext *psc,
												  CJob *pjOwner)
{

	return eevSelfOptimized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::EevtFinalize
//
//	@doc:
//		Finalize optimization
//
//---------------------------------------------------------------------------
CJobGroupExpressionOptimization::EEvent
CJobGroupExpressionOptimization::EevtFinalize(CSchedulerContext *,	// psc
											  CJob *pjOwner)
{

	return eevFinalized;
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::FExecute
//
//	@doc:
//		Main job function
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExpressionOptimization::FExecute(CSchedulerContext *psc)
{
	GPOS_ASSERT(FInit());

	return m_jsm.FRun(psc, this);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::ScheduleJob
//
//	@doc:
//		Schedule a new group expression optimization job
//
//---------------------------------------------------------------------------
void
CJobGroupExpressionOptimization::ScheduleJob(CSchedulerContext *psc,
											 CGroupExpression *pgexpr,
											 COptimizationContext *poc,
											 ULONG ulOptReq, CJob *pjParent)
{
	CJob *pj = psc->Pjf()->PjCreate(CJob::EjtGroupExpressionOptimization);

	// initialize job
	CJobGroupExpressionOptimization *pjgeo = PjConvert(pj);
	pjgeo->Init(pgexpr, poc, ulOptReq);
	psc->Psched()->Add(pjgeo, pjParent);
}


//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::FScheduleCTEOptimization
//
//	@doc:
//		Schedule a new job for CTE optimization
//
//---------------------------------------------------------------------------
BOOL
CJobGroupExpressionOptimization::FScheduleCTEOptimization(
	CSchedulerContext *psc, CGroupExpression *pgexpr, COptimizationContext *poc,
	ULONG ulOptReq, CJob *pjParent)
{

	return true;
}


#ifdef GPOS_DEBUG

//---------------------------------------------------------------------------
//	@function:
//		CJobGroupExpressionOptimization::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CJobGroupExpressionOptimization::OsPrint(IOstream &os) const
{
	os << "Group expr: ";
	m_pgexpr->OsPrint(os);
	os << std::endl;

	return m_jsm.OsHistory(os);
}

#endif	// GPOS_DEBUG

// EOF
