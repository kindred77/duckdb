#include "duckdb_translator/CTranslatorDuckDBToGPORCA.h"
#include "duckdb_translator/CLogicalGetDuckDB.h"

#include "gpos/base.h"
#include "gpopt/exception.h"
#include "gpopt/base/CColumnFactory.h"
#include "gpopt/base/COptCtxt.h"
#include "gpopt/base/CQueryContext.h"
#include "gpopt/metadata/CTableDescriptor.h"
#include "gpopt/metadata/CColumnDescriptor.h"
#include "gpopt/metadata/CName.h"
#include "gpopt/operators/CLogicalProject.h"
#include "gpopt/operators/CLogical.h"
#include "gpopt/base/CColRefTable.h"
#include "gpopt/operators/CScalarProjectList.h"
#include "gpopt/operators/CScalarIdent.h"

#include "naucrates/md/IMDRelation.h"
#include "naucrates/md/IMDColumn.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/CMDIdGPDB.h"

#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "duckdb/planner/operator/logical_projection.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"

using namespace gpopt;
using namespace gpmd;

CTranslatorDuckDBToGPORCA::CTranslatorDuckDBToGPORCA(CMemoryPool *mp, CMDAccessor *mda)
    : m_mp(mp), m_mda(mda) {}
CTranslatorDuckDBToGPORCA::~CTranslatorDuckDBToGPORCA() {}

CExpression *CTranslatorDuckDBToGPORCA::PexprLogicalGet(duckdb::LogicalOperator &op) {
    auto &get = op.Cast<duckdb::LogicalGet>();
    duckdb::optional_ptr<duckdb::TableCatalogEntry> table_entry = get.GetTable();
    GPOS_ASSERT(NULL != table_entry);
    
    // Convert table name
    const char *src = table_entry->name.c_str();
    GP_ULONG len = (GP_ULONG)strlen(src);
    WCHAR *wstr = GPOS_NEW_ARRAY(m_mp, WCHAR, len + 1);
    for (GP_ULONG i = 0; i < len; i++) wstr[i] = src[i];
    wstr[len] = 0;
    
    CName *name = GPOS_NEW(m_mp) CName(m_mp, GPOS_NEW(m_mp) CWStringConst(wstr));
    GPOS_DELETE_ARRAY(wstr);
    
    // Create mdid
    CMDIdGPDB *mdid = GPOS_NEW(m_mp) CMDIdGPDB(0);
    
    // Create table descriptor
    CTableDescriptor *ptabdesc = GPOS_NEW(m_mp) CTableDescriptor(
        m_mp, mdid, *name, false, IMDRelation::EreldistrRandom, IMDRelation::ErelstorageHeap, 0);
    
    // Add columns
    auto &columns = table_entry->GetColumns();
    for (auto &col : columns.Logical()) {
        CName *colname = GPOS_NEW(m_mp) CName(m_mp, GPOS_NEW(m_mp) CWStringConst(L""));
        CColumnDescriptor *coldesc = GPOS_NEW(m_mp) CColumnDescriptor(
            m_mp, NULL, 0, *colname, col.StorageOid(), true);
        ptabdesc->AddColumn(coldesc);
    }
    
    // Create column refs
    CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
    CColRefArray *colrefs = GPOS_NEW(m_mp) CColRefArray(m_mp);
    for (GP_ULONG i = 0; i < ptabdesc->ColumnCount(); i++) {
        auto *coldesc = ptabdesc->Pcoldesc(i);
            // Use minimal PcrCreate (2-arg overload with type and mp)
            // For now just create the column ref with NULL type and use PcrCreate(mp, colref)
        CColRef *colref = GPOS_NEW(m_mp) CColRefTable(coldesc, i, GPOS_NEW(m_mp) CName(m_mp, GPOS_NEW(m_mp) CWStringConst(GPOS_WSZ_LIT("col"))), 0);
        colrefs->Append(colref);
    }
    
    CLogicalGetDuckDB *pop = GPOS_NEW(m_mp) CLogicalGetDuckDB(m_mp, name, ptabdesc, colrefs);
    return GPOS_NEW(m_mp) CExpression(m_mp, pop);
}

CExpression *CTranslatorDuckDBToGPORCA::PexprTranslate(duckdb::LogicalOperator &op) {
    switch (op.type) {
    case duckdb::LogicalOperatorType::LOGICAL_GET:
        return PexprLogicalGet(op);
    default:
        GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnsupportedOp, GPOS_WSZ_LIT("Unsupported op"));
        return NULL;
    }
}

CQueryContext *CTranslatorDuckDBToGPORCA::PqcGenerate(CMemoryPool *mp, CExpression *pexpr) {
    return NULL; // TODO
}
