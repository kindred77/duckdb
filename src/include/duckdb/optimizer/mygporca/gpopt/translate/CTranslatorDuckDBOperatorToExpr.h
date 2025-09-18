//
// Created by admin on 2025/8/13.
//

#ifndef GPOPT_CTranslatorDuckDBOperatorToExpr_H
#define GPOPT_CTranslatorDuckDBOperatorToExpr_H

#include "gpos/base.h"

#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CDuckDBOperator.h"
#include "gpopt/base/CColRef.h"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "duckdb/planner/expression.hpp"
#include "duckdb/common/enums/expression_type.hpp"


namespace gpopt
{
const int INVALID_TYPE_MODIFIER = 0;

class CTranslatorDuckDBOperatorToExpr
{
	// shorthand for functions for translating DXL operator nodes into expression trees
	typedef gpopt::CExpression *(CTranslatorDuckDBOperatorToExpr::*PfPexpr)(
	    duckdb::LogicalOperator *duckOpt);
	typedef gpopt::CExpression *(CTranslatorDuckDBOperatorToExpr::*PfPExpexpr)(
	    duckdb::Expression *duckExp);

	// pair of DXL operator type and the corresponding translator
	struct STranslatorMapping
	{
		// type
		CDuckDBOperator::EDOperatorId eDopid;

		// translator function pointer
		PfPexpr pf;
	};
	struct SExpTranslatorMapping
	{
		// type
		duckdb::ExpressionClass expClass;

		// translator function pointer
		PfPExpexpr pf;
	};
private:
	// memory pool
	gpos::CMemoryPool *m_mp;
	UlongToColRefMap *m_phmulcr;
	CColumnFactory *m_pcf;

	// array of output ColRefId
	ULongPtrArray *m_pdrgpulOutputColRefs;

	// array of output column names
	CMDNameArray *m_pdrgpmdname;

	// DXL operator translators indexed by the operator id
	PfPexpr m_rgpfTranslators[CDuckDBOperator::EDOperatorId::EDopSentinel];
	PfPExpexpr m_rgpfExpTranslators[static_cast<uint8_t>(duckdb::ExpressionClass::BOUND_EXPANDED) + 1];
	void InitTranslators();

	gpopt::CExpression *Pexpr(duckdb::LogicalOperator *op);
	gpopt::CExpression *PexprScalar(duckdb::Expression *expression);

	gpopt::CTableDescriptor *Ptabdesc(duckdb::LogicalGet *get);
	gpopt::CExpression *PexprLogicalGet(duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalFilter(duckdb::LogicalOperator *duckOpt);

	gpopt::CExpression *PexprScalarProjElem(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarProjList(vector<unique_ptr<duckdb::Expression>> &expressions);
	gpopt::CExpression *PexprLogicalProject(duckdb::LogicalOperator *duckOpt);

	gpopt::CExpression *PexprLogicalGroupBy(duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalLimit(duckdb::LogicalOperator *duckOpt);
	gpopt::CExpression *PexprLogicalJoin(duckdb::LogicalOperator *duckOpt);

	// translate children of a expression
	gpopt::CExpressionArray *PdrgpexprChildren(vector<unique_ptr<duckdb::Expression>> &children);

	gpopt::CExpression *PexprScalarIdent(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarCmp(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarOp(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarIsDistinctFrom(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarConst(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarBoolOp(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarFunc(duckdb::Expression *expression);
	gpopt::CExpression *PexprAggFunc(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarNullTest(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarNullIf(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarBooleanTest(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarIf(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarCaseTest(duckdb::Expression *expression);
	gpopt::CExpression *PexprScalarCast(duckdb::Expression *expression);
public:
	ULongPtrArray *PdrgpulOutputColRefs()
	{
		GPOS_ASSERT(NULL != m_pdrgpulOutputColRefs);
		return m_pdrgpulOutputColRefs;
	}

	CMDNameArray *Pdrgpmdname()
	{
		GPOS_ASSERT(NULL != m_pdrgpmdname);
		return m_pdrgpmdname;
	}

	gpopt::CExpression *PexprTranslateQuery(
	    duckdb::LogicalOperator *op,
	    duckdb::vector<duckdb::Expression *> query_output_dxlnode_array,
	    duckdb::vector<duckdb::LogicalCTERef *> cte_refs);
	// ctor
	CTranslatorDuckDBOperatorToExpr(CMemoryPool *mp);

	// dtor
	~CTranslatorDuckDBOperatorToExpr();
};

}
#endif // GPOPT_CTranslatorDuckDBOperatorToExpr_H
