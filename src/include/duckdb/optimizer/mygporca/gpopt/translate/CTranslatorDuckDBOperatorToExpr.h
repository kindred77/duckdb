//
// Created by admin on 2025/8/13.
//

#ifndef GPOPT_CTranslatorDuckDBOperatorToExpr_H
#define GPOPT_CTranslatorDuckDBOperatorToExpr_H

#include "gpos/base.h"

#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CDuckDBOperator.h"
#include "duckdb/planner/operator/logical_order.hpp"

namespace gpopt
{

class CTranslatorDuckDBOperatorToExpr
{
	// shorthand for functions for translating DXL operator nodes into expression trees
	typedef gpopt::CExpression *(CTranslatorDuckDBOperatorToExpr::*PfPexpr)(
	    const duckdb::LogicalOperator *duckOpt);

	// pair of DXL operator type and the corresponding translator
	struct STranslatorMapping
	{
		// type
		CDuckDBOperator::EDOperatorId eDopid;

		// translator function pointer
		PfPexpr pf;
	};
private:
	// memory pool
	gpos::CMemoryPool *m_mp;
	// DXL operator translators indexed by the operator id
	PfPexpr m_rgpfTranslators[CDuckDBOperator::EDOperatorId::EDopSentinel];
	void InitTranslators();
	gpopt::CExpression *Pexpr(duckdb::LogicalOperator *op);

	gpopt::CExpression *PexprLogicalGet(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalSelect(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalProject(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalGroupBy(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalLimit(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalJoin(const duckdb::LogicalOperator *duckOpt);

	gpopt::CExpression *PexprScalarIdent(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarCmp(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarOp(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarIsDistinctFrom(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarConst(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarBoolOp(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarFunc(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprAggFunc(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarNullTest(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarNullIf(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarBooleanTest(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarIf(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarCaseTest(const duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprScalarCast(const duckdb::LogicalOperator *duckOpt);
public:
	gpopt::CExpression *PexprTranslateQuery(duckdb::LogicalOperator *op);
};

}
#endif // GPOPT_CTranslatorDuckDBOperatorToExpr_H
