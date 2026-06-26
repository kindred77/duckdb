//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformFactory.cpp
//
//	@doc:
//		Minimal stub for CXformFactory for DuckDB integration
//---------------------------------------------------------------------------

#include "gpos/base.h"
#include "gpos/memory/CMemoryPoolManager.h"
#include "gpopt/xforms/CXformFactory.h"

using namespace gpopt;

CXformFactory *CXformFactory::m_pxff = NULL;

CXformFactory::CXformFactory(CMemoryPool *mp)
    : m_mp(mp), m_phmszxform(NULL), m_pxfsExploration(NULL),
      m_pxfsImplementation(NULL), m_lastAddedOrSkippedXformId(-1)
{
    GPOS_ASSERT(NULL != mp);
    for (ULONG i = 0; i < CXform::ExfSentinel; i++) {
        m_rgpxf[i] = NULL;
    }
    m_phmszxform = GPOS_NEW(mp) XformNameToXformMap(mp);
    m_pxfsExploration = GPOS_NEW(mp) CXformSet(mp);
    m_pxfsImplementation = GPOS_NEW(mp) CXformSet(mp);
}

CXformFactory::~CXformFactory()
{
    for (ULONG i = 0; i < CXform::ExfSentinel; i++) {
        if (NULL == m_rgpxf[i]) break;
        m_rgpxf[i]->Release();
        m_rgpxf[i] = NULL;
    }
    m_phmszxform->Release();
    m_pxfsExploration->Release();
    m_pxfsImplementation->Release();
}

void CXformFactory::Instantiate() {
    // No xforms instantiated in this minimal stub
}

CXform *CXformFactory::Pxf(CXform::EXformId exfid) const {
    if (exfid < CXform::ExfSentinel) return m_rgpxf[exfid];
    return NULL;
}

CXform *CXformFactory::Pxf(const CHAR *szXformName) const {
    return m_phmszxform->Find(szXformName);
}

BOOL CXformFactory::IsXformIdUsed(CXform::EXformId exfid) {
    return (exfid <= m_lastAddedOrSkippedXformId && NULL != m_rgpxf[exfid]);
}

GPOS_RESULT CXformFactory::Init() {
    GPOS_ASSERT(NULL == Pxff());
    CMemoryPool *mp = CMemoryPoolManager::GetMemoryPoolMgr()->CreateMemoryPool();
    GPOS_TRY {
        m_pxff = GPOS_NEW(mp) CXformFactory(mp);
    }
    GPOS_CATCH_EX(ex) {
        CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(mp);
        m_pxff = NULL;
        if (GPOS_MATCH_EX(ex, CException::ExmaSystem, CException::ExmiOOM))
            return GPOS_OOM;
        return GPOS_FAILED;
    }
    GPOS_CATCH_END;
    m_pxff->Instantiate();
    return GPOS_OK;
}

void CXformFactory::Shutdown() {
    CXformFactory *pxff = CXformFactory::Pxff();
    CMemoryPool *mp = pxff->m_mp;
    CXformFactory::m_pxff = NULL;
    GPOS_DELETE(pxff);
    CMemoryPoolManager::GetMemoryPoolMgr()->Destroy(mp);
}
