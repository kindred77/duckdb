//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CExpression.cpp
//
//	@doc:
//		Implementation of expressions
//---------------------------------------------------------------------------

#include "gpos/base.h"
#include "gpos/error/CAutoTrace.h"
#include "gpos/io/COstreamString.h"
#include "gpos/string/CWStringDynamic.h"
#include "gpos/task/CAutoSuspendAbort.h"
#include "gpos/task/CAutoTraceFlag.h"
#include "gpos/task/CWorker.h"

#include "gpopt/exception.h"
#include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/operators/COperator.h"
#include "gpopt/operators/CPattern.h"
#include "gpopt/operators/CPatternNode.h"
#include "gpopt/search/CGroupExpression.h"
#include "gpopt/statistics/IStatistics.h"
#include "gpopt/traceflags.h"

using namespace gpopt;

static CHAR szExprLevelWS[] = "   ";
static CHAR szExprBarLevelWS[] = "|  ";
static CHAR szExprBarOpPrefix[] = "|--";
static CHAR szExprPlusOpPrefix[] = "+--";


//---------------------------------------------------------------------------
//	@function:
//		CExpression::CExpression
//
//	@doc:
//		Ctor for leaf nodes
//
//---------------------------------------------------------------------------
CExpression::CExpression(CMemoryPool *mp, COperator *pop,
						 CGroupExpression *pgexpr)
	: m_mp(mp),
	  m_pop(pop),
	  m_pdrgpexpr(NULL),
	  m_pgexpr(pgexpr),
	  m_cost(GPOPT_INVALID_COST),
	  m_ulOriginGrpId(gpos::ulong_max),
	  m_ulOriginGrpExprId(gpos::ulong_max)
{
	GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != pop);

//	m_pdprel = GPOS_NEW(m_mp) CDrvdPropRelational(m_mp);
//	m_pdpscalar = GPOS_NEW(m_mp) CDrvdPropScalar(m_mp);
//
//	if (NULL != pgexpr)
//	{
//		CopyGroupPropsAndStats(NULL /*input_stats*/);
//	}
}


//---------------------------------------------------------------------------
//	@function:
//		CExpression::CExpression
//
//	@doc:
//		Ctor, unary
//
//---------------------------------------------------------------------------
CExpression::CExpression(CMemoryPool *mp, COperator *pop, CExpression *pexpr)
	: m_mp(mp),
	  m_pop(pop),
	  m_pdrgpexpr(NULL),
	  m_pgexpr(NULL),
	  m_cost(GPOPT_INVALID_COST),
	  m_ulOriginGrpId(gpos::ulong_max),
	  m_ulOriginGrpExprId(gpos::ulong_max)
{
	GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != pop);
	GPOS_ASSERT(NULL != pexpr);

	//m_pdprel = GPOS_NEW(m_mp) CDrvdPropRelational(m_mp);
	//m_pdpscalar = GPOS_NEW(m_mp) CDrvdPropScalar(m_mp);
	m_pdrgpexpr = GPOS_NEW(mp) CExpressionArray(mp, 1);
	m_pdrgpexpr->Append(pexpr);

	GPOS_ASSERT(m_pdrgpexpr->Size() == 1);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpression::CExpression
//
//	@doc:
//		Ctor, binary
//
//---------------------------------------------------------------------------
CExpression::CExpression(CMemoryPool *mp, COperator *pop,
						 CExpression *pexprChildFirst,
						 CExpression *pexprChildSecond)
	: m_mp(mp),
	  m_pop(pop),
	  m_pdrgpexpr(NULL),
	  m_pgexpr(NULL),
	  m_cost(GPOPT_INVALID_COST),
	  m_ulOriginGrpId(gpos::ulong_max),
	  m_ulOriginGrpExprId(gpos::ulong_max)
{
	GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != pop);

	GPOS_ASSERT(NULL != pexprChildFirst);
	GPOS_ASSERT(NULL != pexprChildSecond);

	//m_pdprel = GPOS_NEW(m_mp) CDrvdPropRelational(m_mp);
	//m_pdpscalar = GPOS_NEW(m_mp) CDrvdPropScalar(m_mp);
	m_pdrgpexpr = GPOS_NEW(mp) CExpressionArray(mp, 2);
	m_pdrgpexpr->Append(pexprChildFirst);
	m_pdrgpexpr->Append(pexprChildSecond);

	GPOS_ASSERT(m_pdrgpexpr->Size() == 2);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpression::CExpression
//
//	@doc:
//		Ctor, ternary
//
//---------------------------------------------------------------------------
CExpression::CExpression(CMemoryPool *mp, COperator *pop,
						 CExpression *pexprChildFirst,
						 CExpression *pexprChildSecond,
						 CExpression *pexprChildThird)
	: m_mp(mp),
	  m_pop(pop),
	  m_pdrgpexpr(NULL),
	  m_pgexpr(NULL),
	  m_cost(GPOPT_INVALID_COST),
	  m_ulOriginGrpId(gpos::ulong_max),
	  m_ulOriginGrpExprId(gpos::ulong_max)
{
	GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != pop);

	GPOS_ASSERT(NULL != pexprChildFirst);
	GPOS_ASSERT(NULL != pexprChildSecond);
	GPOS_ASSERT(NULL != pexprChildThird);

	//m_pdprel = GPOS_NEW(m_mp) CDrvdPropRelational(m_mp);
	//m_pdpscalar = GPOS_NEW(m_mp) CDrvdPropScalar(m_mp);
	m_pdrgpexpr = GPOS_NEW(mp) CExpressionArray(mp, 3);
	m_pdrgpexpr->Append(pexprChildFirst);
	m_pdrgpexpr->Append(pexprChildSecond);
	m_pdrgpexpr->Append(pexprChildThird);

	GPOS_ASSERT(m_pdrgpexpr->Size() == 3);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpression::CExpression
//
//	@doc:
//		Ctor, generic n-ary
//
//---------------------------------------------------------------------------
CExpression::CExpression(CMemoryPool *mp, COperator *pop,
						 CExpressionArray *pdrgpexpr)
	: m_mp(mp),
	  m_pop(pop),
	  m_pdrgpexpr(pdrgpexpr),
	  m_pgexpr(NULL),
	  m_cost(GPOPT_INVALID_COST),
	  m_ulOriginGrpId(gpos::ulong_max),
	  m_ulOriginGrpExprId(gpos::ulong_max)
{
	GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != pop);
	GPOS_ASSERT(NULL != pdrgpexpr);

	//m_pdprel = GPOS_NEW(m_mp) CDrvdPropRelational(m_mp);
	//m_pdpscalar = GPOS_NEW(m_mp) CDrvdPropScalar(m_mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpression::CExpression
//
//	@doc:
//		Ctor, generic n-ary with origin group expression
//
//---------------------------------------------------------------------------
CExpression::CExpression(CMemoryPool *mp, COperator *pop,
						 CGroupExpression *pgexpr, CExpressionArray *pdrgpexpr,
						 IStatistics *input_stats, CCost cost)
	: m_mp(mp),
	  m_pop(pop),
	  m_pdrgpexpr(pdrgpexpr),
	  m_pgexpr(pgexpr),
	  m_cost(cost),
	  m_ulOriginGrpId(gpos::ulong_max),
	  m_ulOriginGrpExprId(gpos::ulong_max)
{
	GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != pop);
	GPOS_ASSERT(pgexpr->Arity() == (pdrgpexpr == NULL ? 0 : pdrgpexpr->Size()));
	GPOS_ASSERT(NULL != pgexpr->Pgroup());

//	m_pdprel = GPOS_NEW(m_mp) CDrvdPropRelational(m_mp);
//	m_pdpscalar = GPOS_NEW(m_mp) CDrvdPropScalar(m_mp);
//
//	CopyGroupPropsAndStats(input_stats);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpression::~CExpression
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CExpression::~CExpression()
{
	{
		CAutoSuspendAbort asa;


		CRefCount::SafeRelease(m_pdrgpexpr);

		m_pop->Release();
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CExpression::FMatchPattern
//
//	@doc:
//		Check a pattern expression against a given group;
//		shallow, do not	match its children, check only arity of the root
//
//---------------------------------------------------------------------------
BOOL
CExpression::FMatchPattern(CGroupExpression *pgexpr) const
{
	GPOS_ASSERT(NULL != pgexpr);

	if (this->Pop()->FPattern())
	{
		if (COperator::EopPatternNode == this->Pop()->Eopid())
		{
			return CPatternNode::PopConvert(this->Pop())
				->MatchesOperator(pgexpr->Pop()->Eopid());
		}
		// a pattern operator other than a CPatternNode matches any group expression
		return true;
	}
	else
	{
		ULONG arity = Arity();
		BOOL fMultiNode =
			((1 == arity || 2 == arity) &&	// has 2 or fewer children
			 CPattern::FMultiNode(
				 (*this)[0]->Pop())	 // child is multileaf or a multitree
			);

		// match operator id and arity
		if (this->Pop()->Eopid() == pgexpr->Pop()->Eopid() &&
			(this->Arity() == pgexpr->Arity() ||
			 (fMultiNode && pgexpr->Arity() > 1)))
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpression::Matches
//
//	@doc:
//		Recursive comparison of this expression against another given one
//
//---------------------------------------------------------------------------
BOOL
CExpression::Matches(CExpression *pexpr) const
{
	GPOS_CHECK_STACK_SIZE;

	// check local operator
	if (!Pop()->Matches(pexpr->Pop()))
	{
		return false;
	}

	ULONG arity = Arity();
	if (arity != pexpr->Arity())
	{
		return false;
	}

	// decend into children
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (!(*this)[ul]->Matches((*pexpr)[ul]))
		{
			return false;
		}
	}

	return true;
}

// EOF
