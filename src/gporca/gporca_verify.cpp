//===----------------------------------------------------------------------===//
//                         DuckDB
//
// gporca_verify.cpp - GPORCA + DuckDB verification test
//===----------------------------------------------------------------------===//

#include <iostream>
#include "gpos/_api.h"
#include "gpos/memory/CAutoMemoryPool.h"
#include "gpos/error/CAutoTrace.h"
#include "gpos/error/CErrorContext.h"
#include "gpos/task/CWorkerPoolManager.h"
#include "gpos/task/CWorker.h"
#include "gpos/task/CTask.h"
#include "gpos/task/CTaskContext.h"

#include "gpopt/init.h"
#include "gpopt/base/CAutoOptCtxt.h"
#include "gpopt/base/COptCtxt.h"
#include "gpopt/base/CColumnFactory.h"
#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/metadata/CTableDescriptor.h"
#include "gpopt/metadata/CColumnDescriptor.h"
#include "gpopt/metadata/CName.h"
#include "gpopt/base/CColRefTable.h"

#include "naucrates/md/CMDIdGPDB.h"
#include "naucrates/md/IMDRelation.h"
#include "naucrates/init.h"

#include "duckdb_translator/CLogicalGetDuckDB.h"

using namespace gpopt;
using namespace gpos;

bool IsAbortRequested() { return false; }

void InitGPOPT() {
    struct gpos_init_params params = {IsAbortRequested};
    gpos_init(&params);
    gpdxl_init();
    gpopt_init();
}

void ShutdownGPOPT() {
    gpopt_terminate();
    gpdxl_terminate();
    gpos_terminate();
}

int main() {
    std::cout << "=== GPORCA + DuckDB verification test ===" << std::endl;

    // Step 1: Initialize GPORCA
    InitGPOPT();

    // Step 2: Create memory pool and worker
    CAutoMemoryPool amp(CAutoMemoryPool::ElcNone);
    CMemoryPool *mp = amp.Pmp();

    CWorker worker(16384, (ULONG_PTR)&worker);
    CTaskContext *task_ctxt = GPOS_NEW(mp) CTaskContext(mp);
    CErrorContext *err_ctxt = GPOS_NEW(mp) CErrorContext();
    CTask *task = GPOS_NEW(mp) CTask(mp, task_ctxt, err_ctxt);
    worker.SetTask(task);

    GPOS_TRY {
        CAutoOptCtxt aoc(mp, NULL, NULL, (COptimizerConfig*)NULL);

        // Create table
        CName *name = GPOS_NEW(mp) CName(mp, GPOS_NEW(mp) CWStringConst(mp, L"test_table"));
        CMDIdGPDB *mdid = GPOS_NEW(mp) CMDIdGPDB(0);
        CTableDescriptor *ptabdesc = GPOS_NEW(mp) CTableDescriptor(
            mp, mdid, *name, false, IMDRelation::EreldistrRandom, IMDRelation::ErelstorageHeap, 0);

        for (int i = 0; i < 3; i++) {
            char col_name[32];
            snprintf(col_name, 32, "col_%d", i + 1);
            GP_ULONG len = (GP_ULONG)strlen(col_name);
            WCHAR *wstr = GPOS_NEW_ARRAY(mp, WCHAR, len + 1);
            for (GP_ULONG j = 0; j < len; j++) wstr[j] = col_name[j];
            wstr[len] = 0;
            CName *colname = GPOS_NEW(mp) CName(mp, GPOS_NEW(mp) CWStringConst(mp, wstr));
            GPOS_DELETE_ARRAY(wstr);
            CColumnDescriptor *coldesc = GPOS_NEW(mp) CColumnDescriptor(mp, NULL, 0, *colname, i + 1, true);
            ptabdesc->AddColumn(coldesc);
        }

        // Create column refs
        CColumnFactory *cf = COptCtxt::PoctxtFromTLS()->Pcf();
        CColRefArray *colrefs = GPOS_NEW(mp) CColRefArray(mp);
        for (GP_ULONG i = 0; i < ptabdesc->ColumnCount(); i++) {
            const CColumnDescriptor *cd = ptabdesc->Pcoldesc(i);
            CColRef *cr = GPOS_NEW(mp) CColRefTable(cd, i, GPOS_NEW(mp) CName(mp, GPOS_NEW(mp) CWStringConst(mp, L"col")), 0);
            colrefs->Append(cr);
        }

        // Create operator
        CLogicalGetDuckDB *pop = GPOS_NEW(mp) CLogicalGetDuckDB(mp, name, ptabdesc, colrefs);
        CExpression *expr = GPOS_NEW(mp) CExpression(mp, pop);

        std::cout << "  Operator: " << pop->SzId() << std::endl;
        std::cout << "  Hash: " << pop->HashValue() << std::endl;

        // Derive properties
        CExpressionHandle exprhdl(mp);
        exprhdl.Attach(expr);
        CColRefSet *output_cols = pop->DeriveOutputColumns(mp, exprhdl);
        std::cout << "  Output cols: " << output_cols->Size() << std::endl;

        CRefCount::SafeRelease(output_cols);
        expr->Release();
        std::cout << "=== Verification PASSED ===" << std::endl;
    }
    GPOS_CATCH_EX(ex) {
        IErrorContext *ect = CTask::Self()->GetErrCtxt();
        std::cerr << "ERROR: " << ex.Filename() << ":" << ex.Line()
                  << " - " << ect->GetErrorMsg() << std::endl;
        ShutdownGPOPT();
        return 1;
    }
    GPOS_CATCH_END;

    GPOS_DELETE(task);
    GPOS_DELETE(err_ctxt);
    GPOS_DELETE(task_ctxt);
    ShutdownGPOPT();
    return 0;
}
