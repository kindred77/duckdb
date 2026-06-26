//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/gporca/duckdb_translator/CLogicalGetDuckDB.h
//
// Wraps DuckDB's LogicalGet as a GPORCA logical operator
//===----------------------------------------------------------------------===//

#ifndef GPOPT_CLogicalGetDuckDB_H
#define GPOPT_CLogicalGetDuckDB_H

#include "gpos/base.h"
#include "gpopt/operators/CLogical.h"
#include "gpopt/metadata/CTableDescriptor.h"

namespace gpopt {

// Forward declarations
class CName;
class CColRefSet;

class CLogicalGetDuckDB : public CLogical {
private:
	// Table alias/name
	const CName *m_pnameAlias;
	
	// GPORCA table descriptor (built from DuckDB catalog info)
	CTableDescriptor *m_ptabdesc;
	
	// Output columns
	CColRefArray *m_pdrgpcrOutput;
	
	CLogicalGetDuckDB(const CLogicalGetDuckDB &);

public:
	// Ctor
	CLogicalGetDuckDB(CMemoryPool *mp, const CName *pnameAlias,
	                  CTableDescriptor *ptabdesc, CColRefArray *pdrgpcrOutput);
	
	virtual ~CLogicalGetDuckDB();
	
	// Op id
	virtual EOperatorId Eopid() const { return EopLogicalGet; }  // Reuse existing
	
	virtual const CHAR *SzId() const { return "CLogicalGetDuckDB"; }
	
	// Accessors
	CColRefArray *PdrgpcrOutput() const { return m_pdrgpcrOutput; }
	const CName &Name() const { return *m_pnameAlias; }
	CTableDescriptor *Ptabdesc() const { return m_ptabdesc; }
	
	// Operator interface
	virtual ULONG HashValue() const;
	virtual BOOL Matches(COperator *pop) const;
	virtual BOOL FInputOrderSensitive() const { return false; }
	virtual COperator *PopCopyWithRemappedColumns(
	    CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);
	
	// Relational properties
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &exprhdl);
	virtual CColRefSet *DeriveNotNullColumns(CMemoryPool *mp, CExpressionHandle &exprhdl) const;
	virtual CPartInfo *DerivePartitionInfo(CMemoryPool *mp, CExpressionHandle &) const {
	    return GPOS_NEW(mp) CPartInfo(mp);
	}
	virtual CPropConstraint *DerivePropertyConstraint(
	    CMemoryPool *mp, CExpressionHandle &) const {
	    return PpcDeriveConstraintFromTable(mp, m_ptabdesc, m_pdrgpcrOutput);
	}
	virtual ULONG DeriveJoinDepth(CMemoryPool *, CExpressionHandle &) const { return 1; }
	virtual CTableDescriptor *DeriveTableDescriptor(CMemoryPool *, CExpressionHandle &) const { return m_ptabdesc; }
	
	// Stat columns
	virtual CColRefSet *PcrsStat(CMemoryPool *, CExpressionHandle &, CColRefSet *, ULONG) const {
	    GPOS_ASSERT(!"CLogicalGetDuckDB has no children");
	    return NULL;
	}
	
	// Stats
	virtual IStatistics *PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl, IStatisticsArray *stats_ctxt) const;
	virtual EStatPromise Esp(CExpressionHandle &) const { return CLogical::EspHigh; }
	
	// Transformations
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;
	
	// Key
	virtual CKeyCollection *DeriveKeyCollection(CMemoryPool *mp, CExpressionHandle &exprhdl) const;
	
	// Debug
	virtual IOstream &OsPrint(IOstream &) const;
	
	static CLogicalGetDuckDB *PopConvert(COperator *pop) {
	    GPOS_ASSERT(NULL != pop);
	    return dynamic_cast<CLogicalGetDuckDB *>(pop);
	}
};

} // namespace gpopt

#endif
