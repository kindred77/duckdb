//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/gporca/duckdb_translator/CTranslatorDuckDBToGPORCA.h
//
// Translates DuckDB LogicalOperator tree to GPORCA CExpression tree
//===----------------------------------------------------------------------===//

#ifndef GPOPT_CTranslatorDuckDBToGPORCA_H
#define GPOPT_CTranslatorDuckDBToGPORCA_H

#include "gpos/base.h"
#include "gpopt/base/CColRef.h"
#include "gpopt/mdcache/CMDAccessor.h"
#include "gpopt/operators/CExpression.h"

namespace duckdb { class LogicalOperator; class ClientContext; }

namespace gpopt {

class CQueryContext;
class CMDProviderDuckDB;

class CTranslatorDuckDBToGPORCA {
private:
	CMemoryPool *m_mp;
	CMDAccessor *m_mda;
	
	CExpression *PexprLogicalGet(duckdb::LogicalOperator &op);
	CExpression *PexprLogicalProject(duckdb::LogicalOperator &op);
	
public:
	CTranslatorDuckDBToGPORCA(CMemoryPool *mp, CMDAccessor *mda);
	virtual ~CTranslatorDuckDBToGPORCA();
	
	CExpression *PexprTranslate(duckdb::LogicalOperator &op);
	CQueryContext *PqcGenerate(CMemoryPool *mp, CExpression *pexpr);
};

} // namespace gpopt

#endif
