#include "gpopt/translate/CTranslatorDuckDBOperatorToExpr.h"

#include "gpopt/exception.h"

#include "gpos/types.h"

using namespace gpopt;
using namespace gpos;

void CTranslatorDuckDBOperatorToExpr::InitTranslators()
{
	// array mapping operator type to translator function
	STranslatorMapping translators_mapping[] = {
	    {CDuckDBOperator::EDOperatorId::EDopLogicalGet,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalGet},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalSelect,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalSelect},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalProject,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalProject},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalGrpBy,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalGroupBy},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalLimit,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalLimit},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalJoin,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalJoin},

	    {CDuckDBOperator::EDOperatorId::EDopScalarIdent,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarIdent},

	    {CDuckDBOperator::EDOperatorId::EDopScalarCmp,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarCmp},

	    {CDuckDBOperator::EDOperatorId::EDopScalarOpExpr,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarOp},

	    {CDuckDBOperator::EDOperatorId::EDopScalarDistinct,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarIsDistinctFrom},

	    {CDuckDBOperator::EDOperatorId::EDopScalarConstValue,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarConst},

	    {CDuckDBOperator::EDOperatorId::EDopScalarBoolExpr,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarBoolOp},

	    {CDuckDBOperator::EDOperatorId::EDopScalarFuncExpr,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarFunc},

	    {CDuckDBOperator::EDOperatorId::EDopScalarAggref,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprAggFunc},

	    {CDuckDBOperator::EDOperatorId::EDopScalarNullTest,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarNullTest},

	    {CDuckDBOperator::EDOperatorId::EDopScalarNullIf,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarNullIf},

	    {CDuckDBOperator::EDOperatorId::EDopScalarBooleanTest,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarBooleanTest},

	    {CDuckDBOperator::EDOperatorId::EDopScalarIfStmt,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarIf},

	    {CDuckDBOperator::EDOperatorId::EDopScalarCaseTest,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarCaseTest},

	    {CDuckDBOperator::EDOperatorId::EDopScalarCast,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarCast},
	};

	const ULONG translators_mapping_len = GPOS_ARRAY_SIZE(translators_mapping);

	for (ULONG ul = 0; ul < translators_mapping_len; ul++)
	{
		STranslatorMapping elem = translators_mapping[ul];
		m_rgpfTranslators[elem.eDopid] = elem.pf;
	}
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprLogicalGet(const LogicalOperator *duckOpt)
{

}

CExpression *
CTranslatorDuckDBOperatorToExpr::Pexpr(duckdb::LogicalOperator *op)
{
	GPOS_CHECK_STACK_SIZE;
	GPOS_ASSERT(NULL != op);

	CExpression *pexpr = NULL;
	switch (op->type)
	{
	//logical
	case LogicalOperatorType::LOGICAL_GET:
		pexpr = PexprLogicalGet(op);
		break;

	default:
		GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnsupportedOp,
		           op->GetName().c_str());
	}

	return pexpr;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprTranslateQuery(duckdb::LogicalOperator *op)
{

}
