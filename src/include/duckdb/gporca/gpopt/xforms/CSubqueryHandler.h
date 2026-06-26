//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CSubqueryHandler.h
//
//	@doc:
//		Helper class for transforming subquery expressions to Apply
//		expressions
//---------------------------------------------------------------------------
#ifndef GPOPT_CSubqueryHandler_H
#define GPOPT_CSubqueryHandler_H

#include "gpos/base.h"

#include "gpopt/operators/CExpression.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CSubqueryHandler
//
//	@doc:
//		Helper class for transforming subquery expressions to Apply
//		expressions
//
//---------------------------------------------------------------------------
class CSubqueryHandler
{
public:
	// context in which subquery appears
	enum ESubqueryCtxt
	{
		EsqctxtValue,  // subquery appears in a project list
		EsqctxtFilter  // subquery appears in a comparison predicate
	};

private:
	//---------------------------------------------------------------------------
	//	@struct:
	//		SSubqueryDesc
	//
	//	@doc:
	//		Structure to maintain subquery descriptor
	//
	//---------------------------------------------------------------------------
	struct SSubqueryDesc
	{
		// subquery can return more than one row
		GP_BOOL m_returns_set;

		// subquery has volatile functions
		GP_BOOL m_fHasVolatileFunctions;

		// subquery has outer references
		GP_BOOL m_fHasOuterRefs;

		// the returned column is an outer reference
		GP_BOOL m_fReturnedPcrIsOuterRef;

		// subquery has skip level correlations -- when inner expression refers to columns defined above the immediate outer expression
		GP_BOOL m_fHasSkipLevelCorrelations;

		// subquery has a single count(*)/count(Any) agg
		GP_BOOL m_fHasCountAgg;

		// column defining count(*)/count(Any) agg, if any
		CColRef *m_pcrCountAgg;

		//  does subquery project a count expression
		GP_BOOL m_fProjectCount;

		// subquery is used in a value context
		GP_BOOL m_fValueSubquery;

		// subquery requires correlated execution
		GP_BOOL m_fCorrelatedExecution;

		// ctor
		SSubqueryDesc()
			: m_returns_set(false),
			  m_fHasVolatileFunctions(false),
			  m_fHasOuterRefs(false),
			  m_fReturnedPcrIsOuterRef(false),
			  m_fHasSkipLevelCorrelations(false),
			  m_fHasCountAgg(false),
			  m_pcrCountAgg(NULL),
			  m_fProjectCount(false),
			  m_fValueSubquery(false),
			  m_fCorrelatedExecution(false)
		{
		}

		// set correlated execution flag
		void SetCorrelatedExecution();

	};	// struct SSubqueryDesc

	// memory pool
	CMemoryPool *m_mp;

	// enforce using correlated apply for unnesting subqueries
	GP_BOOL m_fEnforceCorrelatedApply;

	// private copy ctor
	CSubqueryHandler(const CSubqueryHandler &);

	// helper for adding nullness check, only if needed, to the given scalar expression
	static CExpression *PexprIsNotNull(CMemoryPool *mp, CExpression *pexprOuter,
									   CExpression *pexprLogical,
									   CExpression *pexprScalar);

	// helper for adding a Project node with a const TRUE on top of the given expression
	static void AddProjectNode(CMemoryPool *mp, CExpression *pexpr,
							   CExpression **ppexprResult);

	// helper for creating a groupby node above or below the apply
	static CExpression *CreateGroupByNode(CMemoryPool *mp,
										  CExpression *pexprChild,
										  CColRefArray *colref_array,
										  GP_BOOL fExistential, CColRef *colref,
										  CExpression *pexprPredicate,
										  CColRef **pcrCount, CColRef **pcrSum);

	// helper for creating an inner select expression when creating outer apply
	static CExpression *PexprInnerSelect(CMemoryPool *mp,
										 const CColRef *pcrInner,
										 CExpression *pexprInner,
										 CExpression *pexprPredicate,
										 GP_BOOL *useNotNullableInnerOpt);

	// helper for creating outer apply expression for scalar subqueries
	static GP_BOOL FCreateOuterApplyForScalarSubquery(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
		CExpression *pexprSubquery, GP_BOOL fOuterRefsUnderInner,
		CExpression **ppexprNewOuter, CExpression **ppexprResidualScalar);

	// helper for creating grouping columns for outer apply expression
	static GP_BOOL FCreateGrpCols(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
		GP_BOOL fExistential, GP_BOOL fOuterRefsUnderInner,
		CColRefArray **ppdrgpcr,  // output: constructed grouping columns
		GP_BOOL *pfGbOnInner		  // output: is Gb created on inner expression
	);

	// helper for creating outer apply expression for existential/quantified subqueries
	static GP_BOOL FCreateOuterApplyForExistOrQuant(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
		CExpression *pexprSubquery, CExpression *pexprPredicate,
		GP_BOOL fOuterRefsUnderInner, CExpression **ppexprNewOuter,
		CExpression **ppexprResidualScalar, GP_BOOL useNotNullableInnerOpt);

	// helper for creating outer apply expression
	static GP_BOOL FCreateOuterApply(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprInner,
		CExpression *pexprSubquery, CExpression *pexprPredicate,
		GP_BOOL fOuterRefsUnderInner, CExpression **ppexprNewOuter,
		CExpression **ppexprResidualScalar, GP_BOOL useNotNullableInnerOpt);

	// helper for creating a scalar if expression used when generating an outer apply
	static CExpression *PexprScalarIf(CMemoryPool *mp, CColRef *pcrBool,
									  CColRef *pcrSum, CColRef *pcrCount,
									  CExpression *pexprSubquery,
									  GP_BOOL useNotNullableInnerOpt);

	// helper for creating a correlated apply expression for existential subquery
	static GP_BOOL FCreateCorrelatedApplyForExistentialSubquery(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprSubquery,
		ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter,
		CExpression **ppexprResidualScalar);

	// helper for creating a correlated apply expression for quantified subquery
	static GP_BOOL FCreateCorrelatedApplyForQuantifiedSubquery(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprSubquery,
		ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter,
		CExpression **ppexprResidualScalar);

	// helper for creating correlated apply expression
	static GP_BOOL FCreateCorrelatedApplyForExistOrQuant(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprSubquery,
		ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter,
		CExpression **ppexprResidualScalar);

	// create subquery descriptor
	static SSubqueryDesc *Psd(CMemoryPool *mp, CExpression *pexprSubquery,
							  CExpression *pexprOuter,
							  const CColRef *pcrSubquery,
							  ESubqueryCtxt esqctxt);

	// detect subqueries with expressions over count aggregate similar to
	// (SELECT 'abc' || (SELECT count(*) from X))
	static GP_BOOL FProjectCountSubquery(CExpression *pexprSubquery,
									  CColRef *ppcrCount);

	// given an input expression, replace all occurrences of given column with the given scalar expression
	static CExpression *PexprReplace(CMemoryPool *mp, CExpression *pexpr,
									 CColRef *colref,
									 CExpression *pexprSubquery);

	// remove a scalar subquery node from scalar tree
	GP_BOOL FRemoveScalarSubquery(CExpression *pexprOuter,
							   CExpression *pexprSubquery,
							   ESubqueryCtxt esqctxt,
							   CExpression **ppexprNewOuter,
							   CExpression **ppexprResidualScalar);

	// helper to generate a correlated apply expression when needed
	static GP_BOOL FGenerateCorrelatedApplyForScalarSubquery(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprSubquery,
		ESubqueryCtxt esqctxt, CSubqueryHandler::SSubqueryDesc *psd,
		GP_BOOL fEnforceCorrelatedApply, CExpression **ppexprNewOuter,
		CExpression **ppexprResidualScalar);

	// internal function for removing a scalar subquery node from scalar tree
	static GP_BOOL FRemoveScalarSubqueryInternal(
		CMemoryPool *mp, CExpression *pexprOuter, CExpression *pexprSubquery,
		ESubqueryCtxt esqctxt, SSubqueryDesc *psd, GP_BOOL fEnforceCorrelatedApply,
		CExpression **ppexprNewOuter, CExpression **ppexprResidualScalar);

	// remove a subquery ANY node from scalar tree
	GP_BOOL FRemoveAnySubquery(CExpression *pexprOuter, CExpression *pexprSubquery,
							ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter,
							CExpression **ppexprResidualScalar);

	// remove a subquery ALL node from scalar tree
	GP_BOOL FRemoveAllSubquery(CExpression *pexprOuter, CExpression *pexprSubquery,
							ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter,
							CExpression **ppexprResidualScalar);

	// add a limit 1 expression over given expression,
	// removing any existing limits
	static CExpression *AddOrReplaceLimitOne(CMemoryPool *mp,
											 CExpression *pexpr);

	// remove a subquery EXISTS/NOT EXISTS node from scalar tree
	static GP_BOOL FRemoveExistentialSubquery(
		CMemoryPool *mp, COperator::EOperatorId op_id, CExpression *pexprOuter,
		CExpression *pexprSubquery, ESubqueryCtxt esqctxt,
		CExpression **ppexprNewOuter, CExpression **ppexprResidualScalar);

	// remove a subquery EXISTS from scalar tree
	GP_BOOL FRemoveExistsSubquery(CExpression *pexprOuter,
							   CExpression *pexprSubquery,
							   ESubqueryCtxt esqctxt,
							   CExpression **ppexprNewOuter,
							   CExpression **ppexprResidualScalar);

	// remove a subquery NOT EXISTS from scalar tree
	GP_BOOL FRemoveNotExistsSubquery(CExpression *pexprOuter,
								  CExpression *pexprSubquery,
								  ESubqueryCtxt esqctxt,
								  CExpression **ppexprNewOuter,
								  CExpression **ppexprResidualScalar);

	// handle subqueries in scalar tree recursively
	GP_BOOL FRecursiveHandler(CExpression *pexprOuter, CExpression *pexprScalar,
						   ESubqueryCtxt esqctxt, CExpression **ppexprNewOuter,
						   CExpression **ppexprNewScalar);

	// handle subqueries on a case-by-case basis
	GP_BOOL FProcessScalarOperator(CExpression *pexprOuter,
								CExpression *pexprScalar, ESubqueryCtxt esqctxt,
								CExpression **ppexprNewOuter,
								CExpression **ppexprNewScalar);

#ifdef GPOS_DEBUG
	// assert valid values of arguments
	static void AssertValidArguments(CMemoryPool *mp, CExpression *pexprOuter,
									 CExpression *pexprScalar,
									 CExpression **ppexprNewOuter,
									 CExpression **ppexprResidualScalar);
#endif	// GPOS_DEBUG

public:
	// ctor
	CSubqueryHandler(CMemoryPool *mp, GP_BOOL fEnforceCorrelatedApply)
		: m_mp(mp), m_fEnforceCorrelatedApply(fEnforceCorrelatedApply)
	{
	}

	// build an expression for the quantified comparison of the subquery
	CExpression *PexprSubqueryPred(CExpression *pexprOuter,
								   CExpression *pexprSubquery,
								   CExpression **ppexprResult,
								   CSubqueryHandler::ESubqueryCtxt esqctxt);

	// main driver
	GP_BOOL FProcess(
		CExpression *pexprOuter,   // logical child of a SELECT node
		CExpression *pexprScalar,  // scalar child of a SELECT node
		ESubqueryCtxt esqctxt,	   // context in which subquery occurs
		CExpression *
			*ppexprNewOuter,  // an Apply logical expression produced as output
		CExpression **
			ppexprResidualScalar  // residual scalar expression produced as output
	);

};	// class CSubqueryHandler

}  // namespace gpopt

#endif	// !GPOPT_CSubqueryHandler_H

// EOF
