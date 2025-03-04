#pragma once

namespace duckdb
{

class CTranslatorQueryToDXL
{
public:
    CDXLNode *TranslateFromExprToDXL(unique_ptr<BoundTableRef> from_expr);

    CDXLNode *TranslateSelectQueryToDXL(BoundSelectNode &statement);
};

}
