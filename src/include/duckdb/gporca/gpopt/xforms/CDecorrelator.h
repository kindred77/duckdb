//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDecorrelator.h
//
//	@doc:
//		Decorrelation processor
//---------------------------------------------------------------------------
#ifndef GPOPT_CDecorrelator_H
#define GPOPT_CDecorrelator_H

#include "gpos/base.h"

#include "gpopt/operators/CExpression.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CDecorrelator
//
//	@doc:
//		Helper class for extracting correlated expressions
//
//---------------------------------------------------------------------------
class CDecorrelator
{
private:
	// definition of operator processor
	typedef GP_BOOL(FnProcessor)(CMemoryPool *, CExpression *, GP_BOOL,
							  CExpression **, CExpressionArray *, CColRefSet *);

	//---------------------------------------------------------------------------
	//	@struct:
	//		SOperatorProcessor
	//
	//	@doc:
	//		Mapping of operator to a processor function
	//
	//---------------------------------------------------------------------------
	struct SOperatorProcessor
	{
		// scalar operator id
		COperator::EOperatorId m_eopid;

		// pointer to handler function
		FnProcessor *m_pfnp;

	};	// struct SOperatorHandler

	// private ctor
	CDecorrelator();

	// private dtor
	virtual ~CDecorrelator();

	// private copy ctor
	CDecorrelator(const CDecorrelator &);

	// helper to check if correlations below join are valid to be pulled-up
	static GP_BOOL FPullableCorrelations(CMemoryPool *mp, CExpression *pexpr,
									  CExpressionArray *pdrgpexpr,
									  CExpressionArray *pdrgpexprCorrelations);

	// check if scalar operator can be delayed
	static GP_BOOL FDelayableScalarOp(CExpression *pexprScalar);

	// check if scalar expression can be lifted
	static GP_BOOL FDelayable(CExpression *pexprLogical, CExpression *pexprScalar,
						   GP_BOOL fEqualityOnly);

	// switch function for all operators
	static GP_BOOL FProcessOperator(CMemoryPool *mp, CExpression *pexpr,
								 GP_BOOL fEqualityOnly,
								 CExpression **ppexprDecorrelated,
								 CExpressionArray *pdrgpexprCorrelations,
								 CColRefSet *outerRefsToRemove);

	// processor for predicates
	static GP_BOOL FProcessPredicate(CMemoryPool *mp, CExpression *pexprLogical,
								  CExpression *pexprScalar, GP_BOOL fEqualityOnly,
								  CExpression **ppexprDecorrelated,
								  CExpressionArray *pdrgpexprCorrelations,
								  CColRefSet *outerRefsToRemove);

	// processor for select operators
	static GP_BOOL FProcessSelect(CMemoryPool *mp, CExpression *pexpr,
							   GP_BOOL fEqualityOnly,
							   CExpression **ppexprDecorrelated,
							   CExpressionArray *pdrgpexprCorrelations,
							   CColRefSet *outerRefsToRemove);


	// processor for aggregates
	static GP_BOOL FProcessGbAgg(CMemoryPool *mp, CExpression *pexpr,
							  GP_BOOL fEqualityOnly,
							  CExpression **ppexprDecorrelated,
							  CExpressionArray *pdrgpexprCorrelations,
							  CColRefSet *outerRefsToRemove);

	// processor for joins (inner/n-ary)
	static GP_BOOL FProcessJoin(CMemoryPool *mp, CExpression *pexpr,
							 GP_BOOL fEqualityOnly,
							 CExpression **ppexprDecorrelated,
							 CExpressionArray *pdrgpexprCorrelations,
							 CColRefSet *outerRefsToRemove);


	// processor for projects
	static GP_BOOL FProcessProject(CMemoryPool *mp, CExpression *pexpr,
								GP_BOOL fEqualityOnly,
								CExpression **ppexprDecorrelated,
								CExpressionArray *pdrgpexprCorrelations,
								CColRefSet *outerRefsToRemove);

	// processor for assert
	static GP_BOOL FProcessAssert(CMemoryPool *mp, CExpression *pexpr,
							   GP_BOOL fEqualityOnly,
							   CExpression **ppexprDecorrelated,
							   CExpressionArray *pdrgpexprCorrelations,
							   CColRefSet *outerRefsToRemove);

	// processor for MaxOneRow
	static GP_BOOL FProcessMaxOneRow(CMemoryPool *mp, CExpression *pexpr,
								  GP_BOOL fEqualityOnly,
								  CExpression **ppexprDecorrelated,
								  CExpressionArray *pdrgpexprCorrelations,
								  CColRefSet *outerRefsToRemove);

	// processor for limits
	static GP_BOOL FProcessLimit(CMemoryPool *mp, CExpression *pexpr,
							  GP_BOOL fEqualityOnly,
							  CExpression **ppexprDecorrelated,
							  CExpressionArray *pdrgpexprCorrelations,
							  CColRefSet *outerRefsToRemove);

public:
	// main handler
	static GP_BOOL FProcess(CMemoryPool *mp, CExpression *pexprOrig,
						 GP_BOOL fEqualityOnly, CExpression **ppexprDecorrelated,
						 CExpressionArray *pdrgpexprCorrelations,
						 CColRefSet *outerRefsToRemove);

};	// class CDecorrelator

}  // namespace gpopt

#endif	// !GPOPT_CDecorrelator_H

// EOF
