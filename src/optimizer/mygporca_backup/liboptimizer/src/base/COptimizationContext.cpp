//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COptimizationContext.cpp
//
//	@doc:
//		Implementation of optimization context
//---------------------------------------------------------------------------

#include "gpopt/base/COptimizationContext.h"

#include "gpos/base.h"
#include "gpos/error/CAutoTrace.h"

#include "gpopt/search/CGroupExpression.h"


using namespace gpopt;

FORCE_GENERATE_DBGSTR(COptimizationContext);

// invalid optimization context
const COptimizationContext COptimizationContext::m_ocInvalid;

// invalid optimization context pointer
const OPTCTXT_PTR COptimizationContext::m_pocInvalid = NULL;



//---------------------------------------------------------------------------
//	@function:
//		COptimizationContext::~COptimizationContext
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
COptimizationContext::~COptimizationContext()
{
	//CRefCount::SafeRelease(m_prpp);
	//CRefCount::SafeRelease(m_prprel);
	//CRefCount::SafeRelease(m_pdrgpstatCtxt);
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizationContext::SetBest
//
//	@doc:
//		 Set best cost context
//
//---------------------------------------------------------------------------
void
COptimizationContext::SetBest(CCostContext *pcc)
{
	GPOS_ASSERT(NULL != pcc);

	//m_pccBest = pcc;

//	COperator *pop = pcc->Pgexpr()->Pop();
//	if (CUtils::FPhysicalAgg(pop) &&
//		CPhysicalAgg::PopConvert(pop)->FMultiStage())
//	{
//		m_fHasMultiStageAggPlan = true;
//	}
}


//---------------------------------------------------------------------------
//	@function:
//		COptimizationContext::Matches
//
//	@doc:
//		Match against another context
//
//---------------------------------------------------------------------------
BOOL
COptimizationContext::Matches(const COptimizationContext *poc) const
{
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		COptimizationContext::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
COptimizationContext::OsPrint(IOstream &os) const
{
	return OsPrintWithPrefix(os, "");
}

// EOF
