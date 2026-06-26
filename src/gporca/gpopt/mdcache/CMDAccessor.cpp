#include "gpopt/mdcache/CMDAccessor.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDCast.h"

using namespace gpopt;
using namespace gpmd;

CMDAccessor::CMDAccessor(CMemoryPool *mp, MDCache *pcache)
    : m_mp(mp), m_pcache(pcache), m_pmdpGeneric(NULL),
      m_dLookupTime(0.0), m_dFetchTime(0.0)
{}

CMDAccessor::CMDAccessor(CMemoryPool *mp, MDCache *pcache, CSystemId sysid,
                         IMDProvider *pmdp)
    : m_mp(mp), m_pcache(pcache), m_pmdpGeneric(NULL),
      m_dLookupTime(0.0), m_dFetchTime(0.0)
{ (void)sysid; (void)pmdp; }

CMDAccessor::CMDAccessor(CMemoryPool *mp, MDCache *pcache,
                         const CSystemIdArray *pdrgpsysid,
                         const CMDProviderArray *pdrgpmdp)
    : m_mp(mp), m_pcache(pcache), m_pmdpGeneric(NULL),
      m_dLookupTime(0.0), m_dFetchTime(0.0)
{ (void)pdrgpsysid; (void)pdrgpmdp; }

CMDAccessor::~CMDAccessor() {}

const IMDScalarOp *CMDAccessor::RetrieveScOp(IMDId *mdid) {
    GPOS_ASSERT(!"CMDAccessor::RetrieveScOp stub");
    (void)mdid; return NULL;
}

const IMDType *CMDAccessor::RetrieveType(IMDId *mdid) {
    GPOS_ASSERT(!"CMDAccessor::RetrieveType stub");
    (void)mdid; return NULL;
}

const IMDCast *CMDAccessor::Pmdcast(IMDId *mdid_src, IMDId *mdid_dest) {
    GPOS_ASSERT(!"CMDAccessor::Pmdcast stub");
    (void)mdid_src; (void)mdid_dest; return NULL;
}
