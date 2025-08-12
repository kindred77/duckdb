//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CQueryContext.h
//
//	@doc:
//		A container for query-specific input objects to the optimizer
//---------------------------------------------------------------------------
#ifndef DUCKDB_CQUERYCONTEXT_H
#define DUCKDB_CQUERYCONTEXT_H

#include "gpos/base.h"
#include "gpos/common/DbgPrintMixin.h"

#include "gpopt/search/CGroupExpression.h"


namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CQueryContext
//
//	@doc:
//		Query specific information that optimizer receives as input
//		representing the requirements that need to be satisfied by the final
//		plan. This includes:
//		- Input logical expression
//		- Required columns
//		- Required plan (physical) properties at the top level of the query.
//		  This will include sort order, rewindability etc requested by the entire
//		  query.
//
//		The function CQueryContext::PqcGenerate() is the main routine that
//		generates a query context object for a given logical expression and
//		required output columns. See there for more details of how
//		CQueryContext is constructed.
//
//		NB: One instance of CQueryContext is created per query. It is then used
//		to initialize the CEngine.
//
//
//---------------------------------------------------------------------------
class CQueryContext : public DbgPrintMixin<CQueryContext>
{
private:

	// logical expression tree to be optimized
	CExpression *m_pexpr;

	// should statistics derivation take place
	BOOL m_fDeriveStats;

	// collect system columns from output columns
	void SetSystemCols(CMemoryPool *mp);

	// return top level operator in the given expression
	static COperator *PopTop(CExpression *pexpr);

	// private copy ctor
	CQueryContext(const CQueryContext &);

public:

	// dtor
	virtual ~CQueryContext();

	BOOL
	FDeriveStats() const
	{
		return m_fDeriveStats;
	}

	// expression accessor
	CExpression *
	Pexpr() const
	{
		return m_pexpr;
	}

};	// class CQueryContext
}  // namespace gpopt


#endif	// !DUCKDB_CQUERYCONTEXT_H

// EOF
