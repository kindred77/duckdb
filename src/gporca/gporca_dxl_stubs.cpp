// GPORCA DXL stubs for DuckDB
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/dxltokens.h"
#include "naucrates/md/CDXLColStats.h"
#include "naucrates/md/CDXLRelStats.h"
#include "gpopt/mdcache/CMDAccessor.h"
#include "gpopt/xforms/CXform.h"
#include "gpopt/xforms/CXformResult.h"
#include "gpopt/operators/CExpressionUtils.h"
#include "naucrates/statistics/CStatistics.h"
#include "gpopt/mdcache/CMDAccessorUtils.h"
#include <cstring>

const gpopt::CWStringConst* gpdxl::CDXLTokens::GetDXLTokenStr(gpdxl::Edxltoken) { return NULL; }
void gpdxl::CXMLSerializer::OpenElement(const CWStringBase*, const CWStringBase*) {}
void gpdxl::CXMLSerializer::CloseElement(const CWStringBase*, const CWStringBase*) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, BOOL) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, ULONG) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, const CWStringBase*) {}
void gpdxl::CDXLNode::SerializeToDXL(CXMLSerializer*) const {}

gpmd::CDXLColStats* gpmd::CDXLColStats::CreateDXLDummyColStats(CMemoryPool*, IMDId*, CMDName*, CDouble) { return NULL; }
gpmd::CDXLRelStats* gpmd::CDXLRelStats::CreateDXLDummyRelStats(CMemoryPool*, IMDId*) { return NULL; }

bool gpopt::CXform::FEqualIds(const CHAR* a, const CHAR* b) { return strcmp(a, b) == 0; }
gpopt::CExpression* gpopt::CXformResult::PexprNext() { return NULL; }
gpopt::CExpression* gpopt::CExpressionUtils::PexprDedupChildren(CMemoryPool* mp, CExpression* p) { (void)mp; return p; }

const gpopt::IMDType* gpopt::CMDAccessor::RetrieveType(gpmd::IMDType::ETypeInfo ti) { (void)ti; return NULL; }
const gpmd::IMDRelation* gpopt::CMDAccessor::RetrieveRel(gpmd::IMDId*) { return NULL; }
const gpmd::IMDFunction* gpopt::CMDAccessor::RetrieveFunc(gpmd::IMDId*) { return NULL; }
gpnaucrates::IStatistics* gpopt::CMDAccessor::Pstats(CMemoryPool*, IMDId*, CColRefSet*, CColRefSet*, CStatisticsConfig*) { return NULL; }
const gpmd::IMDIndex* gpopt::CMDAccessor::RetrieveIndex(gpmd::IMDId*) { return NULL; }
const gpmd::IMDAggregate* gpopt::CMDAccessor::RetrieveAgg(gpmd::IMDId*) { return NULL; }

gpnaucrates::CDXLStatsDerivedRelation* gpnaucrates::CStatistics::GetDxlStatsDrvdRelation(CMemoryPool*, gpopt::CMDAccessor*) const { return NULL; }

gpmd::IMDId* gpmd::IMDProvider::GetGPDBTypeMdid(CMemoryPool*, gpmd::CSystemId, gpmd::IMDType::ETypeInfo) { return NULL; }
void gpmd::IMDCacheObject::SerializeMDIdAsElem(CXMLSerializer*, const CWStringConst*, const IMDId*) const {}
gpos::CHAR* gpdxl::CDXLUtils::Read(CMemoryPool*, const CHAR*) { return NULL; }
CWStringDynamic* gpdxl::CDXLUtils::SerializeMDObj(CMemoryPool*, const IMDCacheObject*, bool, bool) { return NULL; }
IMDCacheObjectArray* gpdxl::CDXLUtils::ParseDXLToIMDObjectArray(CMemoryPool*, const CHAR*, const CHAR*) { return NULL; }

// CScalar stubs
bool gpopt::CScalarIdent::FCastedScId(CExpression* p) { (void)p; return false; }
bool gpopt::CScalarAggFunc::FCountStar() const { return false; }
bool gpopt::CScalarAggFunc::FCountAny() const { return false; }
gpmd::IMDId* gpopt::CScalarAggFunc::MDId() const { return NULL; }
bool gpopt::CScalarProjectList::FHasMultipleDistinctAggs(CExpressionHandle&) { return false; }
bool gpopt::CScalarProjectList::FHasScalarFunc(CExpressionHandle&) { return false; }
ULONG gpopt::CScalarProjectList::UlDistinctAggs(CExpressionHandle&) { return 0; }
ULONG gpopt::CScalarProjectList::UlOrderedAggs(CExpressionHandle&) { return 0; }
gpopt::CScalarConstArray* gpopt::CScalarArray::PdrgPconst() const { return NULL; }
gpmd::IMDId* gpopt::CScalarArray::PmdidElem() const { return NULL; }
gpmd::IMDId* gpopt::CScalarArray::PmdidArray() const { return NULL; }
bool gpopt::CScalarArray::FMultiDimensional() const { return false; }

// CMDAccessorUtils stubs
bool gpopt::CMDAccessorUtils::FCmpOrCastedCmpExists(gpmd::IMDId*, gpmd::IMDId*, gpmd::IMDType::ECmpType) { return false; }
gpmd::IMDId* gpopt::CMDAccessorUtils::GetScCmpMdIdConsiderCasts(CMDAccessor*, CExpression*, CExpression*, gpmd::IMDType::ECmpType) { return NULL; }
void gpopt::CMDAccessorUtils::ApplyCastsForScCmp(CMemoryPool*, CMDAccessor*, CExpression*&, CExpression*&, gpmd::IMDId*) {}

// Misc stubs
gpopt::CXformResult::CXformResult(CMemoryPool* mp) : CRefCount(mp) {}
gpopt::CLogicalInnerJoin::FFewerConj(CMemoryPool*, CGroupExpression*, CGroupExpression*) { return false; }
void gpopt::CXformUtils::FDeriveStatsBeforeXform(CXform*) {}
void gpopt::CXformUtils::FApplyToNextBinding(CXform*, CExpression*) {}
gpopt::CJobTest::CJobTest() {}
gpopt::CExpression* gpopt::CXformResult::PexprNext() { return NULL; }

// Additional DXL stubs
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, INT) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, LINT) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, ULLONG) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, CDouble) {}
gpdxl::CCostModelConfigSerializer::CCostModelConfigSerializer(const gpopt::ICostModel*) {}
void gpdxl::CCostModelConfigSerializer::Serialize(CXMLSerializer&) const {}