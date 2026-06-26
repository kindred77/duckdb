#include "duckdb_translator/CLogicalGetDuckDB.h"

#include "gpos/base.h"
#include "gpopt/base/CColRefSet.h"
#include "gpopt/base/CKeyCollection.h"
#include "gpopt/base/CPartIndexMap.h"
#include "gpopt/base/CReqdPropRelational.h"
#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/xforms/CXformFactory.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CFilterStatsProcessor.h"

using namespace gpopt;

CLogicalGetDuckDB::CLogicalGetDuckDB(CMemoryPool *mp, const CName *pnameAlias,
                                     CTableDescriptor *ptabdesc, CColRefArray *pdrgpcrOutput)
    : CLogical(mp),
      m_pnameAlias(pnameAlias),
      m_ptabdesc(ptabdesc),
      m_pdrgpcrOutput(pdrgpcrOutput)
{
    GPOS_ASSERT(NULL != pnameAlias);
    GPOS_ASSERT(NULL != ptabdesc);
    GPOS_ASSERT(NULL != pdrgpcrOutput);
}

CLogicalGetDuckDB::~CLogicalGetDuckDB() {
    CRefCount::SafeRelease(m_ptabdesc);
    CRefCount::SafeRelease(m_pdrgpcrOutput);
    GPOS_DELETE(m_pnameAlias);
}

ULONG CLogicalGetDuckDB::HashValue() const {
    return gpos::CombineHashes(m_ptabdesc->MDId()->HashValue(),
                               CUtils::UlHashColArray(m_pdrgpcrOutput));
}

BOOL CLogicalGetDuckDB::Matches(COperator *pop) const {
    if (pop->Eopid() != EopLogicalGet)
        return false;
    CLogicalGetDuckDB *popGet = dynamic_cast<CLogicalGetDuckDB *>(pop);
    if (NULL == popGet)
        return false;
    return m_ptabdesc->MDId()->Equals(popGet->Ptabdesc()->MDId()) &&
           m_pdrgpcrOutput->Equals(popGet->PdrgpcrOutput());
}

COperator *CLogicalGetDuckDB::PopCopyWithRemappedColumns(
    CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist) {
    CColRefArray *colref_array = CUtils::PdrgpcrRemap(mp, m_pdrgpcrOutput, colref_mapping, must_exist);
    CName *pnameAlias = GPOS_NEW(mp) CName(mp, *m_pnameAlias);
    m_ptabdesc->AddRef();
    return GPOS_NEW(mp) CLogicalGetDuckDB(mp, pnameAlias, m_ptabdesc, colref_array);
}

CColRefSet *CLogicalGetDuckDB::DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &) {
    return GPOS_NEW(mp) CColRefSet(mp, m_pdrgpcrOutput);
}

CColRefSet *CLogicalGetDuckDB::DeriveNotNullColumns(CMemoryPool *mp, CExpressionHandle &) const {
    CColRefSet *pcrs = GPOS_NEW(mp) CColRefSet(mp);
    const ULONG len = m_pdrgpcrOutput->Size();
    for (ULONG ul = 0; ul < len; ul++) {
        pcrs->Include((*m_pdrgpcrOutput)[ul]);
    }
    return pcrs;
}

CKeyCollection *CLogicalGetDuckDB::DeriveKeyCollection(
    CMemoryPool *mp, CExpressionHandle &) const {
    const CBitSetArray *pdrgpbsKeys = m_ptabdesc->PdrgpbsKeys();
    if (NULL == pdrgpbsKeys || 0 == pdrgpbsKeys->Size())
        return NULL;
    return CLogical::PkcKeysBaseTable(mp, pdrgpbsKeys, m_pdrgpcrOutput);
}

IStatistics *CLogicalGetDuckDB::PstatsDerive(
    CMemoryPool *mp, CExpressionHandle &exprhdl, IStatisticsArray *stats_ctxt) const {
    return CLogical::PstatsBaseTable(mp, exprhdl, m_ptabdesc);
}

CXformSet *CLogicalGetDuckDB::PxfsCandidates(CMemoryPool *mp) const {
    CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
    (void)xform_set->ExchangeSet(CXform::ExfGet2TableScan);
    return xform_set;
}

IOstream &CLogicalGetDuckDB::OsPrint(IOstream &os) const {
    os << "CLogicalGetDuckDB";
    if (NULL != m_pnameAlias) {
        os << " (";
        m_pnameAlias->OsPrint(os);
        os << ")";
    }
    return os;
}
