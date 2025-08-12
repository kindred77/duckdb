//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC CORP.
//
//	@filename:
//		CExpression.h
//
//	@doc:
//		Basic tree/DAG-based representation for an expression
//---------------------------------------------------------------------------
#ifndef DUCKDB_CEXPRESSION_H
#define DUCKDB_CEXPRESSION_H

#include "gpos/base.h"
#include "gpos/common/CDynamicPtrArray.h"
#include "gpos/common/CRefCount.h"
#include "gpos/common/DbgPrintMixin.h"

#include "gpopt/operators/COperator.h"
#include "gpopt/cost/CCost.h"
#include "gpopt/base/CCostContext.h"
#include "gpopt/statistics/IStatistics.h"

namespace gpopt
{
// cleanup function for arrays
class CExpression;
typedef CDynamicPtrArray<CExpression, CleanupRelease> CExpressionArray;

// array of arrays of expression pointers
typedef CDynamicPtrArray<CExpressionArray, CleanupRelease> CExpressionArrays;

class CGroupExpression;

using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CExpression
//
//	@doc:
//		Simply dynamic array for pointer types
//
//---------------------------------------------------------------------------
class CExpression : public CRefCount, public gpos::DbgPrintMixin<CExpression>
{
	friend class CExpressionHandle;

private:
	// memory pool
	CMemoryPool *m_mp;

	// operator class
	COperator *m_pop;

	// array of children
	CExpressionArray *m_pdrgpexpr;

	// group reference to Memo
	CGroupExpression *m_pgexpr;

	// cost of physical expression node when copied out of the memo
	CCost m_cost;

	// id of origin group, used for debugging expressions extracted from memo
	ULONG m_ulOriginGrpId;

	// id of origin group expression, used for debugging expressions extracted from memo
	ULONG m_ulOriginGrpExprId;

	// private copy ctor
	CExpression(const CExpression &);

public:
	// ctor's with different arity

	// ctor for leaf nodes
	CExpression(CMemoryPool *mp, COperator *pop,
				CGroupExpression *pgexpr = NULL);

	// ctor for unary expressions
	CExpression(CMemoryPool *mp, COperator *pop, CExpression *pexpr);

	// ctor for binary expressions
	CExpression(CMemoryPool *mp, COperator *pop, CExpression *pexprChildFirst,
				CExpression *pexprChildSecond);

	// ctor for ternary expressions
	CExpression(CMemoryPool *mp, COperator *pop, CExpression *pexprChildFirst,
				CExpression *pexprChildSecond, CExpression *pexprChildThird);

	// ctor n-ary expressions
	CExpression(CMemoryPool *mp, COperator *pop, CExpressionArray *pdrgpexpr);

	// ctor for n-ary expression with origin group expression
	CExpression(CMemoryPool *mp, COperator *pop, CGroupExpression *pgexpr,
	            CExpressionArray *pdrgpexpr, IStatistics *input_stats,
	            CCost cost = GPOPT_INVALID_COST);

	// dtor
	~CExpression();

	// shorthand to access children
	CExpression *
	operator[](ULONG ulPos) const
	{
		GPOS_ASSERT(NULL != m_pdrgpexpr);
		return (*m_pdrgpexpr)[ulPos];
	};

	// arity function
	ULONG
	Arity() const
	{
		return m_pdrgpexpr == NULL ? 0 : m_pdrgpexpr->Size();
	}

	// accessor for operator
	COperator *
	Pop() const
	{
		GPOS_ASSERT(NULL != m_pop);
		return m_pop;
	}

	// accessor of children array
	CExpressionArray *
	PdrgPexpr() const
	{
		return m_pdrgpexpr;
	}

	// accessor for origin group expression
	CGroupExpression *
	Pgexpr() const
	{
		return m_pgexpr;
	}

	// cost accessor
	CCost
	Cost() const
	{
		return m_cost;
	}

	// reset all derived properties
	//void ResetDerivedProperties();

	// reset expression stats
	//void ResetStats();

	// check for outer references
	//BOOL HasOuterRefs();

	// print driver
	//virtual IOstream &OsPrint(IOstream &os) const;

	// match with group expression
	BOOL FMatchPattern(CGroupExpression *pgexpr) const;

	// compare entire expression rooted here
	BOOL Matches(CExpression *pexpr) const;

};	// class CExpression


// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CExpression &expr)
{
	return expr.OsPrint(os);
}

// hash map from ULONG to expression
typedef CHashMap<ULONG, CExpression, gpos::HashValue<ULONG>,
				 gpos::Equals<ULONG>, CleanupDelete<ULONG>,
				 CleanupRelease<CExpression> >
	UlongToExprMap;

// map iterator
typedef CHashMapIter<ULONG, CExpression, gpos::HashValue<ULONG>,
					 gpos::Equals<ULONG>, CleanupDelete<ULONG>,
					 CleanupRelease<CExpression> >
	UlongToExprMapIter;

}  // namespace gpopt


#endif	// !DUCKDB_CEXPRESSION_H

// EOF
