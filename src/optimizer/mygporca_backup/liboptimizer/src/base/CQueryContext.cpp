//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CQueryContext.cpp
//
//	@doc:
//		Implementation of optimization context
//---------------------------------------------------------------------------

#include "gpopt/base/CQueryContext.h"

#include "gpos/base.h"
#include "gpos/error/CAutoTrace.h"
#include "gpos/common/DbgPrintMixin.h"

using namespace gpopt;

//FORCE_GENERATE_DBGSTR(CQueryContext);


//---------------------------------------------------------------------------
//	@function:
//		CQueryContext::~CQueryContext
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CQueryContext::~CQueryContext()
{
	m_pexpr->Release();
	//m_prpp->Release();
	//m_pdrgpcr->Release();
	//m_pdrgpmdname->Release();
	//CRefCount::SafeRelease(m_pdrgpcrSystemCols);
}


//---------------------------------------------------------------------------
//	@function:
//		CQueryContext::PopTop
//
//	@doc:
// 		 Return top level operator in the given expression
//
//---------------------------------------------------------------------------
COperator *
CQueryContext::PopTop(CExpression *pexpr)
{
	GPOS_ASSERT(NULL != pexpr);

	// skip CTE anchors if any
	CExpression *pexprCurr = pexpr;
//	while (COperator::EopLogicalCTEAnchor == pexprCurr->Pop()->Eopid())
//	{
//		pexprCurr = (*pexprCurr)[0];
//		GPOS_ASSERT(NULL != pexprCurr);
//	}

	return pexprCurr->Pop();
}

//---------------------------------------------------------------------------
//	@function:
//		CQueryContext::SetReqdSystemCols
//
//	@doc:
// 		Collect system columns from output columns
//
//---------------------------------------------------------------------------
void
CQueryContext::SetSystemCols(CMemoryPool *mp)
{

}

// EOF
