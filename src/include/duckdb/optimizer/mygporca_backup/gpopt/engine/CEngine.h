//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC Corp.
//
//	@filename:
//		CEngine.h
//
//	@doc:
//		Optimization engine
//---------------------------------------------------------------------------
#ifndef DUCKDB_CENGINE_H
#define DUCKDB_CENGINE_H

#include "gpos/base.h"

#include "gpopt/search/CMemo.h"
#include "gpopt/search/CSearchStage.h"
#include "gpopt/xforms/CXform.h"

namespace gpopt
{
using namespace gpos;


// forward declarations
class CGroup;
class CExpression;
class CJob;
class CJobFactory;
class CQueryContext;

//---------------------------------------------------------------------------
//	@class:
//		CEngine
//
//	@doc:
//		Optimization engine; owns entire optimization workflow
//
//---------------------------------------------------------------------------
class CEngine : public DbgPrintMixin<CEngine>
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// query context
	CQueryContext *m_pqc;

	// search strategy
	CSearchStageArray *m_search_stage_array;

	// index of current search stage
	ULONG m_ulCurrSearchStage;

	// memo table
	CMemo *m_pmemo;

	//  pattern used for adding enforcers
	CExpression *m_pexprEnforcerPattern;

	// the following variables are used for maintaining optimization statistics

	// set of activated xforms
	CXformSet *m_xforms;

	// initialize query logical expression
	void InitLogicalExpression(CExpression *pexpr);

	// insert children of the given expression to memo, and
	// copy the resulting groups to the given group array
	void InsertExpressionChildren(CExpression *pexpr,
								  CGroupArray *pdrgpgroupChildren,
								  CXform::EXformId exfidOrigin,
								  CGroupExpression *pgexprOrigin);

	// create and schedule the main optimization job
	void ScheduleMainJob(CSchedulerContext *psc, COptimizationContext *poc);

	// process trace flags after optimization is complete
	void ProcessTraceFlags();

	// check if search has terminated
	BOOL
	FSearchTerminated() const
	{
		// at least one stage has completed and achieved required cost
		return (NULL != PssPrevious() && PssPrevious()->FAchievedReqdCost());
	}

	// sample possible plans uniformly
	void SamplePlans();

	// inaccessible copy ctor
	CEngine(const CEngine &);

public:
	// ctor
	explicit CEngine(CMemoryPool *mp);

	// dtor
	~CEngine();

	// initialize engine with a query context and search strategy
	void Init(CQueryContext *pqc, CSearchStageArray *search_stage_array);

	// accessor of memo's root group
	CGroup *
	PgroupRoot() const
	{
		GPOS_ASSERT(NULL != m_pmemo);

		return m_pmemo->PgroupRoot();
	}

	// check if a group is the root one
	BOOL
	FRoot(CGroup *pgroup) const
	{
		return (PgroupRoot() == pgroup);
	}

	// insert expression tree to memo
	CGroup *PgroupInsert(CGroup *pgroupTarget, CExpression *pexpr,
						 CXform::EXformId exfidOrigin,
						 CGroupExpression *pgexprOrigin, BOOL fIntermediate);

	// execute operations after search stage completes
	void FinalizeSearchStage();

	// main driver of optimization engine
	void Optimize();

	// merge duplicate groups
	void
	GroupMerge()
	{
		m_pmemo->GroupMerge();
	}

	// query context accessor
	const CQueryContext *
	Pqc() const
	{
		return m_pqc;
	}

	// return current search stage
	CSearchStage *
	PssCurrent() const
	{
		return (*m_search_stage_array)[m_ulCurrSearchStage];
	}

	// current search stage index accessor
	ULONG
	UlCurrSearchStage() const
	{
		return m_ulCurrSearchStage;
	}

	// return previous search stage
	CSearchStage *
	PssPrevious() const
	{
		if (0 == m_ulCurrSearchStage)
		{
			return NULL;
		}

		return (*m_search_stage_array)[m_ulCurrSearchStage - 1];
	}

	// number of search stages accessor
	ULONG
	UlSearchStages() const
	{
		return m_search_stage_array->Size();
	}

	// set of xforms of current stage
	CXformSet *
	PxfsCurrentStage() const
	{
		return (*m_search_stage_array)[m_ulCurrSearchStage]->GetXformSet();
	}

	// print
	IOstream &OsPrint(IOstream &) const;

	// return the first group expression in a given group
	static CGroupExpression *PgexprFirst(CGroup *pgroup);

};	// class CEngine

// shorthand for printing
inline IOstream &
operator<<(IOstream &os, const CEngine &eng)
{
	return eng.OsPrint(os);
}
}  // namespace gpopt

#endif	// !DUCKDB_CENGINE_H


// EOF
