#include "duckdb/optimizer/mygporca/CTranslatorQueryToDXL.h"

namespace duckdb
{

CDXLNode *
CTranslatorQueryToDXL::TranslateFromExprToDXL(unique_ptr<BoundTableRef> ref)
{
    switch (ref.type) {
	case TableReferenceType::BASE_TABLE:
		//root = CreatePlan(ref.Cast<BoundBaseTableRef>());
		break;
	case TableReferenceType::SUBQUERY:
		//root = CreatePlan(ref.Cast<BoundSubqueryRef>());
		break;
	case TableReferenceType::JOIN:
		//root = CreatePlan(ref.Cast<BoundJoinRef>());
		break;
	case TableReferenceType::EMPTY_FROM:
		//root = CreatePlan(ref.Cast<BoundEmptyTableRef>());
		break;
	case TableReferenceType::INVALID:
	default:
		throw InternalException("Unsupported bound table ref type (%s)", EnumUtil::ToString(ref.type));
	}
}

CDXLNode *
CTranslatorQueryToDXL::TranslateSelectQueryToDXL(BoundSelectNode &statement)
{
    if (statement.sample_options)
    {

    }

    if (statement.where_clause)
    {

    }

    if (!statement.aggregates.empty() || !statement.groups.group_expressions.empty())
    {

    }

    if (statement.having)
    {

    }

    if (!statement.windows.empty())
    {

    }
}

}