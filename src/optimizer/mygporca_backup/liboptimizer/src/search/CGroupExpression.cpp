//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CGroupExpression.cpp
//
//	@doc:
//		Implementation of group expressions
//---------------------------------------------------------------------------

#include "gpopt/search/CGroupExpression.h"

#include "gpos/base.h"
#include "gpos/error/CAutoTrace.h"
#include "gpos/io/COstreamString.h"
#include "gpos/string/CWStringDynamic.h"
#include "gpos/task/CAutoSuspendAbort.h"
#include "gpos/task/CWorker.h"

#include "gpopt/xforms/CXformFactory.h"
#include "gpopt/xforms/CXform.h"
#include "gpopt/search/CBinding.h"
#include "gpopt/traceflags.h"

using namespace gpopt;

FORCE_GENERATE_DBGSTR(CGroupExpression);

#define GPOPT_COSTCTXT_HT_BUCKETS 100

// invalid group expression
const CGroupExpression CGroupExpression::m_gexprInvalid;


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::CGroupExpression
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CGroupExpression::CGroupExpression(CMemoryPool *mp, COperator *pop,
								   CGroupArray *pdrgpgroup,
								   CXform::EXformId exfid,
								   CGroupExpression *pgexprOrigin,
								   BOOL fIntermediate)
	: m_id(GPOPT_INVALID_GEXPR_ID),
	  m_pgexprDuplicate(NULL),
	  m_pop(pop),
	  m_pdrgpgroup(pdrgpgroup),
	  m_pdrgpgroupSorted(NULL),
	  m_pgroup(NULL),
	  m_exfidOrigin(exfid),
	  m_pgexprOrigin(pgexprOrigin),
	  m_fIntermediate(fIntermediate),
	  m_estate(estUnexplored),
	  m_eol(EolLow),
	  m_ecirculardependency(ecdDefault)
{
	GPOS_ASSERT(NULL != pop);
	GPOS_ASSERT(NULL != pdrgpgroup);
	GPOS_ASSERT_IMP(exfid != CXform::ExfInvalid, NULL != pgexprOrigin);

	// store sorted array of children for faster comparison
	if (1 < pdrgpgroup->Size() && !pop->FInputOrderSensitive())
	{
		m_pdrgpgroupSorted = GPOS_NEW(mp) CGroupArray(mp, pdrgpgroup->Size());
		m_pdrgpgroupSorted->AppendArray(pdrgpgroup);
		m_pdrgpgroupSorted->Sort();

		GPOS_ASSERT(m_pdrgpgroupSorted->IsSorted());
	}

}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::~CGroupExpression
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CGroupExpression::~CGroupExpression()
{
	if (this != &(CGroupExpression::m_gexprInvalid))
	{
		//CleanupContexts();

		m_pop->Release();
		m_pdrgpgroup->Release();

		CRefCount::SafeRelease(m_pdrgpgroupSorted);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::Init
//
//	@doc:
//		Init group expression
//
//
//---------------------------------------------------------------------------
void
CGroupExpression::Init(CGroup *pgroup, ULONG id)
{
	SetGroup(pgroup);
	SetId(id);
	//SetOptimizationLevel();
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::SetId
//
//	@doc:
//		Set id of expression
//
//---------------------------------------------------------------------------
void
CGroupExpression::SetId(ULONG id)
{
	GPOS_ASSERT(GPOPT_INVALID_GEXPR_ID == m_id);

	m_id = id;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::SetGroup
//
//	@doc:
//		Set group pointer of expression
//
//---------------------------------------------------------------------------
void
CGroupExpression::SetGroup(CGroup *pgroup)
{
	GPOS_ASSERT(NULL == m_pgroup);
	GPOS_ASSERT(NULL != pgroup);

	m_pgroup = pgroup;
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::PostprocessTransform
//
//	@doc:
//		Post-processing after applying transformation
//
//---------------------------------------------------------------------------
void
CGroupExpression::PostprocessTransform(CMemoryPool *,  // pmpLocal
									   CMemoryPool *,  // pmpGlobal
									   CXform *pxform)
{

}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::Transform
//
//	@doc:
//		Transform group expression using the given xform
//
//---------------------------------------------------------------------------
void
CGroupExpression::Transform(
	CMemoryPool *mp, CMemoryPool *pmpLocal, CXform *pxform,
	CXformResult *pxfres,
	ULONG *pulElapsedTime,	// output: elapsed time in millisecond
	ULONG *pulNumberOfBindings)
{
	GPOS_ASSERT(NULL != pulElapsedTime);
	GPOS_CHECK_ABORT;

	BOOL fPrintOptStats = false;//GPOS_FTRACE(EopttracePrintOptimizationStatistics);
	CTimerUser timer;
	if (fPrintOptStats)
	{
		timer.Restart();
	}

	*pulElapsedTime = 0;
	// check traceflag and compatibility with origin xform
	if (GPOPT_FDISABLED_XFORM(pxform->Exfid()) ||
		!pxform->FCompatible(m_exfidOrigin))
	{
		if (fPrintOptStats)
		{
			*pulElapsedTime = timer.ElapsedMS();
		}
		return;
	}

	// check xform promise
	//CExpressionHandle exprhdl(mp);
	//exprhdl.Attach(this);
	//exprhdl.DeriveProps(NULL /*pdpctxt*/);
//	if (CXform::ExfpNone == pxform->Exfp(exprhdl))
//	{
//		if (GPOS_FTRACE(EopttracePrintOptimizationStatistics))
//		{
//			*pulElapsedTime = timer.ElapsedMS();
//		}
//		return;
//	}

	// pre-processing before applying xform to group expression
	//PreprocessTransform(pmpLocal, mp, pxform);

	// extract memo bindings to apply xform
	CBinding binding;
	CXformContext *pxfctxt = GPOS_NEW(mp) CXformContext(mp);

//	COptimizerConfig *optconfig =
//		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
//	ULONG bindThreshold = optconfig->GetHint()->UlXformBindThreshold();
	CExpression *pexprPattern = pxform->PexprPattern();
	CExpression *pexpr = binding.PexprExtract(mp, this, pexprPattern, NULL);
	while (NULL != pexpr)
	{
		++(*pulNumberOfBindings);
		ULONG ulNumResults = pxfres->Pdrgpexpr()->Size();
		pxform->Transform(pxfctxt, pxfres, pexpr);
		ulNumResults = pxfres->Pdrgpexpr()->Size() - ulNumResults;
		//PrintXform(mp, pxform, pexpr, pxfres, ulNumResults);

//		if ((bindThreshold != 0 && (*pulNumberOfBindings) > bindThreshold) ||
//			pxform->IsApplyOnce() ||
//			(0 < pxfres->Pdrgpexpr()->Size() &&
//			 !CXformUtils::FApplyToNextBinding(pxform, pexpr)))
//		{
//			// do not apply xform to other possible patterns
//			pexpr->Release();
//			break;
//		}

		CExpression *pexprLast = pexpr;
		pexpr = binding.PexprExtract(mp, this, pexprPattern, pexprLast);

		// release last extracted expression
		pexprLast->Release();

		GPOS_CHECK_ABORT;
	}
	pxfctxt->Release();

	// post-prcoessing before applying xform to group expression
	PostprocessTransform(pmpLocal, mp, pxform);

	if (fPrintOptStats)
	{
		*pulElapsedTime = timer.ElapsedMS();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CGroupExpression::Matches
//
//	@doc:
//		Match group expression against given operator and its children
//
//---------------------------------------------------------------------------
BOOL
CGroupExpression::Matches(const CGroupExpression *pgexpr) const
{
	GPOS_ASSERT(NULL != pgexpr);

	// make sure we are not comparing to invalid group expression
	if (NULL == this->Pop() || NULL == pgexpr->Pop())
	{
		return NULL == this->Pop() && NULL == pgexpr->Pop();
	}

	// have same arity
	if (Arity() != pgexpr->Arity())
	{
		return false;
	}

	// match operators
	if (!m_pop->Matches(pgexpr->m_pop))
	{
		return false;
	}

	// compare inputs
	if (0 == Arity())
	{
		return true;
	}
	else
	{
		if (1 == Arity() || m_pop->FInputOrderSensitive())
		{
			return CGroup::FMatchGroups(m_pdrgpgroup, pgexpr->m_pdrgpgroup);
		}
		else
		{
			GPOS_ASSERT(NULL != m_pdrgpgroupSorted &&
						NULL != pgexpr->m_pdrgpgroupSorted);

			return CGroup::FMatchGroups(m_pdrgpgroupSorted,
										pgexpr->m_pdrgpgroupSorted);
		}
	}

	GPOS_ASSERT(!"Unexpected exit from function");
	return false;
}

// EOF
