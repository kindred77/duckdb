//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 -2011 EMC Corp.
//
//	@filename:
//		CGroup.cpp
//
//	@doc:
//		Implementation of Memo groups; database agnostic
//---------------------------------------------------------------------------

#include "gpopt/search/CGroup.h"

#include "gpos/base.h"
#include "gpos/types.h"
#include "gpos/error/CAutoTrace.h"
#include "gpos/task/CAutoSuspendAbort.h"
#include "gpos/task/CAutoTraceFlag.h"
#include "gpos/task/CWorker.h"

#include "gpopt/base/COptimizationContext.h"
#include "gpopt/exception.h"
#include "gpopt/operators/COperator.h"
#include "gpopt/search/CGroupProxy.h"
#include "gpopt/search/CJobGroup.h"
#include "gpopt/search/CGroupExpression.h"

using namespace gpopt;

FORCE_GENERATE_DBGSTR(CGroup);

#define GPOPT_OPTCTXT_HT_BUCKETS 100

//---------------------------------------------------------------------------
//	@function:
//		CGroup::CGroup
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CGroup::CGroup(CMemoryPool *mp, BOOL fScalar)
	: m_mp(mp),
	  m_id(GPOPT_INVALID_GROUP_ID),
	  m_fScalar(fScalar),
	  m_pdrgpexprJoinKeysOuter(NULL),
	  m_pdrgpexprJoinKeysInner(NULL),
	  m_pexprScalarRep(NULL),
	  m_pexprScalarRepIsExact(false),
	  m_pgroupDuplicate(NULL),
	  m_plinkmap(NULL),
	  m_ulGExprs(0),
	  m_ulpOptCtxts(0),
	  m_estate(estUnexplored),
	  m_eolMax(EolLow),
	  m_fHasNewLogicalOperators(false),
	  m_ulCTEProducerId(gpos::ulong_max),
	  m_fCTEConsumer(false)
{
	GPOS_ASSERT(NULL != mp);

	m_listGExprs.Init(GPOS_OFFSET(CGroupExpression, m_linkGroup));
	m_listDupGExprs.Init(GPOS_OFFSET(CGroupExpression, m_linkGroup));

	m_sht.Init(
		mp, GPOPT_OPTCTXT_HT_BUCKETS, GPOS_OFFSET(COptimizationContext, m_link),
		0, /*cKeyOffset (0 because we use COptimizationContext class as key)*/
		&(COptimizationContext::m_ocInvalid), NULL,
		COptimizationContext::Equals);
	m_plinkmap = GPOS_NEW(mp) LinkMap(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::~CGroup
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CGroup::~CGroup()
{
	CRefCount::SafeRelease(m_pdrgpexprJoinKeysOuter);
	CRefCount::SafeRelease(m_pdrgpexprJoinKeysInner);
	//CRefCount::SafeRelease(m_join_opfamilies);
	//CRefCount::SafeRelease(m_pdp);
	CRefCount::SafeRelease(m_pexprScalarRep);
	//CRefCount::SafeRelease(m_pccDummy);
	//CRefCount::SafeRelease(m_pstats);
	m_plinkmap->Release();
	//m_pstatsmap->Release();
	//m_pcostmap->Release();

	// cleaning-up group expressions
	CGroupExpression *pgexpr = m_listGExprs.First();
	while (NULL != pgexpr)
	{
		CGroupExpression *pgexprNext = m_listGExprs.Next(pgexpr);
		//pgexpr->CleanupContexts();
		pgexpr->Release();

		pgexpr = pgexprNext;
	}

	// cleaning-up duplicate expressions
	pgexpr = m_listDupGExprs.First();
	while (NULL != pgexpr)
	{
		CGroupExpression *pgexprNext = m_listDupGExprs.Next(pgexpr);
		//pgexpr->CleanupContexts();
		pgexpr->Release();

		pgexpr = pgexprNext;
	}

	// cleanup optimization contexts
	CleanupContexts();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::CleanupContexts
//
//	@doc:
//		 Destroy stored contexts in hash table
//
//---------------------------------------------------------------------------
void
CGroup::CleanupContexts()
{
	// need to suspend cancellation while cleaning up
	{
		CAutoSuspendAbort asa;

		COptimizationContext *poc = NULL;
		ShtIter shtit(m_sht);

		while (NULL != poc || shtit.Advance())
		{
			CRefCount::SafeRelease(poc);

			// iter's accessor scope
			{
				ShtAccIter shtitacc(shtit);
				if (NULL != (poc = shtitacc.Value()))
				{
					shtitacc.Remove(poc);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::Ppoc
//
//	@doc:
//		Lookup a context by id
//
//---------------------------------------------------------------------------
COptimizationContext *
CGroup::Ppoc(ULONG id) const
{
	COptimizationContext *poc = NULL;
	ShtIter shtit(const_cast<CGroup *>(this)->m_sht);
	while (shtit.Advance())
	{
		{
			ShtAccIter shtitacc(shtit);
			poc = shtitacc.Value();

			if (poc->Id() == id)
			{
				return poc;
			}
		}
	}
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PocInsert
//
//	@doc:
//		Insert a given context into contexts hash table only if a matching
//		context does not already exist;
//		return either the inserted or the existing matching context
//
//---------------------------------------------------------------------------
COptimizationContext *
CGroup::PocInsert(COptimizationContext *poc)
{
	ShtAcc shta(Sht(), *poc);

	COptimizationContext *pocFound = shta.Find();
	if (NULL == pocFound)
	{
		poc->SetId((ULONG) UlpIncOptCtxts());
		shta.Insert(poc);
		return poc;
	}

	return pocFound;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprBest
//
//	@doc:
//		Lookup best group expression under optimization context
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprBest(COptimizationContext *poc)
{
	ShtAcc shta(Sht(), *poc);
	COptimizationContext *pocFound = shta.Find();
	if (NULL != pocFound)
	{
		return pocFound->PgexprBest();
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SetId
//
//	@doc:
//		Set group id;
//		separated from constructor to avoid synchronization issues
//
//---------------------------------------------------------------------------
void
CGroup::SetId(ULONG id)
{
	GPOS_ASSERT(GPOPT_INVALID_GROUP_ID == m_id &&
				"Overwriting previously assigned group id");

	m_id = id;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::SetState
//
//	@doc:
//		Set group state;
//
//---------------------------------------------------------------------------
void
CGroup::SetState(EState estNewState)
{
	GPOS_ASSERT(estNewState == (EState)(m_estate + 1));

	m_estate = estNewState;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::HashValue
//
//	@doc:
//		Hash function for group identification
//
//---------------------------------------------------------------------------
ULONG
CGroup::HashValue() const
{
	ULONG id = m_id;
	if (FDuplicateGroup() && 0 == m_ulGExprs)
	{
		// group has been merged into another group
		id = PgroupDuplicate()->Id();
	}

	return gpos::HashValue<ULONG>(&id);
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::MoveDuplicateGExpr
//
//	@doc:
//		Move duplicate group expression to duplicates list
//
//---------------------------------------------------------------------------
void
CGroup::MoveDuplicateGExpr(CGroupExpression *pgexpr)
{
	m_listGExprs.Remove(pgexpr);
	m_ulGExprs--;

	m_listDupGExprs.Append(pgexpr);
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprFirst
//
//	@doc:
//		Retrieve first expression in group
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprFirst()
{
	return m_listGExprs.First();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::PgexprNext
//
//	@doc:
//		Retrieve next expression in group
//
//---------------------------------------------------------------------------
CGroupExpression *
CGroup::PgexprNext(CGroupExpression *pgexpr)
{
	return m_listGExprs.Next(pgexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FMatchGroups
//
//	@doc:
//		Determine whether two arrays of groups are equivalent
//
//---------------------------------------------------------------------------
BOOL
CGroup::FMatchGroups(CGroupArray *pdrgpgroupFst, CGroupArray *pdrgpgroupSnd)
{
	ULONG arity = pdrgpgroupFst->Size();
	GPOS_ASSERT(pdrgpgroupSnd->Size() == arity);

	for (ULONG i = 0; i < arity; i++)
	{
		CGroup *pgroupFst = (*pdrgpgroupFst)[i];
		CGroup *pgroupSnd = (*pdrgpgroupSnd)[i];
		if (pgroupFst != pgroupSnd && !FDuplicateGroups(pgroupFst, pgroupSnd))
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::FMatchNonScalarGroups
//
//	@doc:
//		 Matching of pairs of arrays of groups while skipping scalar groups
//
//---------------------------------------------------------------------------
BOOL
CGroup::FMatchNonScalarGroups(CGroupArray *pdrgpgroupFst,
							  CGroupArray *pdrgpgroupSnd)
{
	GPOS_ASSERT(NULL != pdrgpgroupFst);
	GPOS_ASSERT(NULL != pdrgpgroupSnd);

	if (pdrgpgroupFst->Size() != pdrgpgroupSnd->Size())
	{
		return false;
	}

	ULONG arity = pdrgpgroupFst->Size();
	GPOS_ASSERT(pdrgpgroupSnd->Size() == arity);

	for (ULONG i = 0; i < arity; i++)
	{
		CGroup *pgroupFst = (*pdrgpgroupFst)[i];
		CGroup *pgroupSnd = (*pdrgpgroupSnd)[i];
		if (pgroupFst->FScalar())
		{
			// skip scalar groups
			continue;
		}

		if (pgroupFst != pgroupSnd && !FDuplicateGroups(pgroupFst, pgroupSnd))
		{
			return false;
		}
	}

	return true;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::FDuplicateGroups
//
//	@doc:
//		Determine whether two groups are equivalent
//
//---------------------------------------------------------------------------
BOOL
CGroup::FDuplicateGroups(CGroup *pgroupFst, CGroup *pgroupSnd)
{
	GPOS_ASSERT(NULL != pgroupFst);
	GPOS_ASSERT(NULL != pgroupSnd);

	CGroup *pgroupFstDup = pgroupFst->PgroupDuplicate();
	CGroup *pgroupSndDup = pgroupSnd->PgroupDuplicate();

	return (pgroupFst == pgroupSnd) ||	// pointer equality
		   (pgroupFst ==
			pgroupSndDup) ||  // first group is duplicate of second group
		   (pgroupSnd ==
			pgroupFstDup) ||  // second group is duplicate of first group
		   // both groups have the same duplicate group
		   (NULL != pgroupFstDup && NULL != pgroupSndDup &&
			pgroupFstDup == pgroupSndDup);
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::AddDuplicateGrp
//
//	@doc:
//		Add duplicate group
//
//---------------------------------------------------------------------------
void
CGroup::AddDuplicateGrp(CGroup *pgroup)
{
	GPOS_ASSERT(NULL != pgroup);

	// add link following monotonic ordering of group IDs
	CGroup *pgroupSrc = this;
	CGroup *pgroupDest = pgroup;
	if (pgroupSrc->Id() > pgroupDest->Id())
	{
		std::swap(pgroupSrc, pgroupDest);
	}

	// keep looping until we add link
	while (pgroupSrc->m_pgroupDuplicate != pgroupDest)
	{
		if (NULL == pgroupSrc->m_pgroupDuplicate)
		{
			pgroupSrc->m_pgroupDuplicate = pgroupDest;
		}
		else
		{
			pgroupSrc = pgroupSrc->m_pgroupDuplicate;
			if (pgroupSrc->Id() > pgroupDest->Id())
			{
				std::swap(pgroupSrc, pgroupDest);
			}
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResolveDuplicateMaster
//
//	@doc:
//		Resolve master duplicate group
//
//---------------------------------------------------------------------------
void
CGroup::ResolveDuplicateMaster()
{
	if (!FDuplicateGroup())
	{
		return;
	}
	CGroup *pgroupTarget = m_pgroupDuplicate;
	while (NULL != pgroupTarget->m_pgroupDuplicate)
	{
		GPOS_ASSERT(pgroupTarget->Id() < pgroupTarget->m_pgroupDuplicate->Id());
		pgroupTarget = pgroupTarget->m_pgroupDuplicate;
	}

	// update reference to target group
	m_pgroupDuplicate = pgroupTarget;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroup::MergeGroup
//
//	@doc:
//		Merge group with its duplicate - not thread-safe
//
//---------------------------------------------------------------------------
void
CGroup::MergeGroup()
{
	if (!FDuplicateGroup())
	{
		return;
	}
	GPOS_ASSERT(FExplored());
	GPOS_ASSERT(!FImplemented());

	// resolve target group
	ResolveDuplicateMaster();
	CGroup *pgroupTarget = m_pgroupDuplicate;

	// move group expressions from this group to target
	while (!m_listGExprs.IsEmpty())
	{
		CGroupExpression *pgexpr = m_listGExprs.RemoveHead();
		m_ulGExprs--;

		pgexpr->Reset(pgroupTarget, pgroupTarget->m_ulGExprs++);
		pgroupTarget->Insert(pgexpr);

		GPOS_CHECK_ABORT;
	}

	GPOS_ASSERT(0 == m_ulGExprs);
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetGroupState
//
//	@doc:
//		Reset group state;
//		resetting state is not thread-safe
//
//---------------------------------------------------------------------------
void
CGroup::ResetGroupState()
{
	// reset group expression states
	CGroupExpression *pgexpr = m_listGExprs.First();
	while (NULL != pgexpr)
	{
		//pgexpr->ResetState();
		pgexpr = m_listGExprs.Next(pgexpr);

		GPOS_CHECK_ABORT;
	}

	// reset group state
	{
		CGroupProxy gp(this);
		m_estate = estUnexplored;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetLinkMap
//
//	@doc:
//		Reset link map for plan enumeration;
//		this operation is not thread safe
//
//
//---------------------------------------------------------------------------
void
CGroup::ResetLinkMap()
{
	GPOS_ASSERT(NULL != m_plinkmap);

	m_plinkmap->Release();
	m_plinkmap = GPOS_NEW(m_mp) LinkMap(m_mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CGroup::ResetGroupJobQueues
//
//	@doc:
//		Reset group job queues;
//
//---------------------------------------------------------------------------
void
CGroup::ResetGroupJobQueues()
{
	CGroupProxy gp(this);
	m_jqExploration.Reset();
	m_jqImplementation.Reset();
}

// EOF
