#include "gpopt/translate/CTranslatorDuckDBOperatorToExpr.h"

#include "gpos/types.h"

#include "gpopt/exception.h"
#include "gpopt/base/COptCtxt.h"
#include "gpopt/translate/CUtil.h"
#include "gpopt/operators/CScalarProjectList.h"
#include "gpopt/operators/CScalarProjectElement.h"

#include "duckdb/parser/column_list.hpp"
#include "duckdb/planner/operator/logical_filter.hpp"
#include "duckdb/planner/operator/logical_projection.hpp"
#include "duckdb/planner/operator/logical_aggregate.hpp"
#include "duckdb/planner/operator/logical_join.hpp"
#include "duckdb/planner/expression/bound_conjunction_expression.hpp"
#include "duckdb/common/helper.hpp"

using namespace gpopt;
using namespace gpos;
using namespace duckdb;

void CTranslatorDuckDBOperatorToExpr::InitTranslators()
{
	// array mapping operator type to translator function
	STranslatorMapping translators_mapping[] = {
	    {CDuckDBOperator::EDOperatorId::EDopLogicalGet,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalGet},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalFilter,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalFilter},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalProject,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalProject},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalGrpBy,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalGroupBy},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalLimit,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalLimit},

	    {CDuckDBOperator::EDOperatorId::EDopLogicalJoin,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprLogicalJoin},
	};

	SExpTranslatorMapping exp_translators_mapping[] = {
	    {ExpressionClass::BOUND_COLUMN_REF,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarIdent},

	    {ExpressionClass::BOUND_COMPARISON,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarCmp},

	    {ExpressionClass::BOUND_OPERATOR,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarOp},

	    //{CDuckDBOperator::EDOperatorId::EDopScalarDistinct,
	    // &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarIsDistinctFrom},

	    {ExpressionClass::BOUND_CONSTANT,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarConst},

	    {ExpressionClass::BOUND_CONJUNCTION,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarBoolOp},

	    {ExpressionClass::BOUND_FUNCTION,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarFunc},

	    {ExpressionClass::BOUND_AGGREGATE,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprAggFunc},

	    //{CDuckDBOperator::EDOperatorId::EDopScalarNullTest,
	    // &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarNullTest},

	    //{CDuckDBOperator::EDOperatorId::EDopScalarNullIf,
	    // &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarNullIf},

	    //{CDuckDBOperator::EDOperatorId::EDopScalarBooleanTest,
	    // &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarBooleanTest},

	    //{CDuckDBOperator::EDOperatorId::EDopScalarIfStmt,
	    // &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarIf},

	    {ExpressionClass::BOUND_CASE,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarCaseTest},

	    {ExpressionClass::BOUND_CAST,
	     &gpopt::CTranslatorDuckDBOperatorToExpr::PexprScalarCast},
	};

	const ULONG translators_mapping_len = GPOS_ARRAY_SIZE(translators_mapping);

	for (ULONG ul = 0; ul < translators_mapping_len; ul++)
	{
		STranslatorMapping elem = translators_mapping[ul];
		m_rgpfTranslators[elem.eDopid] = elem.pf;
	}

	const ULONG exp_translators_mapping_len = GPOS_ARRAY_SIZE(exp_translators_mapping);

	for (ULONG ul = 0; ul < exp_translators_mapping_len; ul++)
	{
		SExpTranslatorMapping elem = exp_translators_mapping[ul];
		m_rgpfExpTranslators[elem.expClass] = elem.pf;
	}
}

CTranslatorDuckDBOperatorToExpr::CTranslatorDuckDBOperatorToExpr(CMemoryPool *mp)
	: m_mp(mp)
{
	m_phmulcr = GPOS_NEW(m_mp) UlongToColRefMap(m_mp);
	m_pcf = COptCtxt::PoctxtFromTLS()->Pcf();
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprLogicalGet(LogicalOperator *duckOpt)
{
	//according to duckdb, LogicalGet has only one child.
	CExpression *pexpr = GPOS_NEW(m_mp) CExpression(m_mp, duckOpt);
	return pexpr;
}

void
unsplitFilterExpressionsExp(LogicalFilter &logical_filter)
{
	if (logical_filter.expressions.size() <= 1)
	{
		return;
	}
	unique_ptr<BoundConjunctionExpression> result = nullptr;
	auto &expressions = logical_filter.expressions;
	for (auto &expression : expressions)
	{
		if (!result)
		{
			result = make_uniq<BoundConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
			                                               std::move(expressions[0]), std::move(expression));
		}
		else
		{
			result = make_uniq<BoundConjunctionExpression>(
			    ExpressionType::CONJUNCTION_AND, std::move(result), std::move(expression));
		}
	}
	logical_filter.expressions[0] = std::move(result);
	logical_filter.expressions.resize(1);
}

/**
 * expressions in LogicalFilter would be shinked to one conjunction expression
 * @param duckOpt
 * @return
 */
gpopt::CExpression *
CTranslatorDuckDBOperatorToExpr::PexprLogicalFilter(LogicalOperator *duckOpt)
{
	auto *child = Pexpr(duckOpt->children[0].get());
	LogicalFilter &logical_filter = duckOpt->Cast<LogicalFilter>();
	CExpression *cond_expression = NULL;
	if (logical_filter.expressions.size() > 0)
	{
		//unsplit filter expressions to one conjunction expression
		unsplitFilterExpressionsExp(logical_filter);
		cond_expression = PexprScalar(logical_filter.expressions[0].get());
	}
	CExpression *pexprFilter =
	    GPOS_NEW(m_mp) CExpression(m_mp, duckOpt, child, cond_expression);
	return pexprFilter;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprScalarProjElem(Expression *expression)
{
	CName *name = CUtil::CreateNameFromCharArray(m_mp, expression->GetAlias().c_str());

	CExpression *pexprChild = PexprScalar(expression);
	// generate a new column reference
	CColRef *colref =
	    m_pcf->PcrCreate(CUtil::GetMdType(m_mp, expression->return_type), INVALID_TYPE_MODIFIER, *name);

	//m_phmulcr->Insert(GPOS_NEW(m_mp) ULONG(pdxlopPrEl->Id()), colref);

	CExpression *pexprProjElem = GPOS_NEW(m_mp) CExpression(
	    m_mp, GPOS_NEW(m_mp) CScalarProjectElement(m_mp, colref), pexprChild);
	return pexprProjElem;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprScalarProjList(vector<unique_ptr<Expression>> &expressions)
{
	CExpression *pexprProjList = NULL;
	if (expressions.size() == 0)
	{
		pexprProjList = GPOS_NEW(m_mp)
		    CExpression(m_mp, GPOS_NEW(m_mp) CScalarProjectList(m_mp));
	}
	else
	{
		CExpressionArray *pdrgpexprProjElems =
		    GPOS_NEW(m_mp) CExpressionArray(m_mp);
		for (ULONG ul = 0; ul < expressions.size(); ul++)
		{
			CExpression *pexprProjElem = PexprScalarProjElem(expressions[ul].get());
			pdrgpexprProjElems->Append(pexprProjElem);
		}

		pexprProjList = GPOS_NEW(m_mp) CExpression(
		    m_mp, GPOS_NEW(m_mp) CScalarProjectList(m_mp), pdrgpexprProjElems);
	}
	return pexprProjList;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprLogicalProject(LogicalOperator *duckOpt)
{
	auto *child = Pexpr(duckOpt->children[0].get());

	CExpression *pexprProjList = PexprScalarProjList(duckOpt->expressions);

	CExpression *pexprProject =
	    GPOS_NEW(m_mp) CExpression(m_mp, duckOpt, child, pexprProjList);

	return pexprProject;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprLogicalGroupBy(LogicalOperator *duckOpt)
{
	auto *child = Pexpr(duckOpt->children[0].get());
	LogicalAggregate &logical_agg = duckOpt->Cast<LogicalAggregate>();
	//aggregate projects
	auto *pexprAggProjList = PexprScalarProjList(logical_agg.expressions);
	CExpression *pexprAgg =
	    GPOS_NEW(m_mp) CExpression(m_mp, duckOpt, child, pexprAggProjList);

	return pexprAgg;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprLogicalJoin(LogicalOperator *duckOpt)
{
	LogicalJoin &logical_join = duckOpt->Cast<LogicalJoin>();
	switch (logical_join.join_type)
	{
	case JoinType::INNER:
		return GPOS_NEW(m_mp) CExpression(
		    m_mp, duckOpt,
		    Pexpr(logical_join.children[0].get()),
		    Pexpr(logical_join.children[1].get()));\
	default:
		GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnsupportedOp,
		           logical_join.join_type);
	}
}

CExpression *
CTranslatorDuckDBOperatorToExpr::Pexpr(LogicalOperator *op)
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
	case LogicalOperatorType::LOGICAL_FILTER:
		pexpr = PexprLogicalFilter(op);
		break;
	case LogicalOperatorType::LOGICAL_PROJECTION:
		pexpr = PexprLogicalProject(op);
		break;
	case LogicalOperatorType::LOGICAL_AGGREGATE_AND_GROUP_BY:
		pexpr = PexprLogicalGroupBy(op);
		break;
	default:
		GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnsupportedOp,
		           op->GetName().c_str());
	}

	return pexpr;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprScalarBoolOp(Expression *expression)
{

}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprScalar(Expression *expression)
{
	GPOS_CHECK_STACK_SIZE;
	GPOS_ASSERT(NULL != expression);

	CExpression *pexpr = NULL;
	switch (expression->GetExpressionClass())
	{
	//logical
	case ExpressionClass::BOUND_AGGREGATE:
		pexpr = PexprAggFunc(expression);
		break;
	case ExpressionClass::BOUND_COLUMN_REF:
		pexpr = PexprScalarIdent(expression);
		break;
	case ExpressionClass::BOUND_CONJUNCTION:
		pexpr = PexprScalarBoolOp(expression);
		break;
	default:
		GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnsupportedOp,
		           expression->GetExpressionClass());
	}

	return pexpr;
}

CExpression *
CTranslatorDuckDBOperatorToExpr::PexprTranslateQuery(duckdb::LogicalOperator *op)
{

}

// dtor
CTranslatorDuckDBOperatorToExpr::~CTranslatorDuckDBOperatorToExpr()
{

}
