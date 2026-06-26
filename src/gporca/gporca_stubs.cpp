// GPORCA stubs for DuckDB
#include <cstring>
#include "gpos/base.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "gpopt/operators/CPhysicalSort.h"
#include "gpopt/operators/CPhysicalMotionGather.h"
#include "gpopt/operators/CPhysicalMotionRoutedDistribute.h"
#include "gpopt/operators/CPhysicalSpool.h"
#include "gpopt/operators/CPhysicalPartitionSelector.h"
#include "gpopt/operators/CPhysicalUnionAll.h"
#include "gpopt/operators/CPhysicalDynamicScan.h"
#include "gpopt/operators/CLogicalSelect.h"
#include "gpopt/operators/CLogicalProject.h"
#include "gpopt/operators/CLogicalGbAgg.h"
#include "gpopt/operators/CLogicalSequenceProject.h"
#include "gpopt/operators/CScalarProjectList.h"
#include "gpopt/operators/CScalarAggFunc.h"
#include "gpopt/operators/CScalarCoerceViaIO.h"
#include "gpopt/operators/CScalarIdent.h"
#include "naucrates/md/CDXLColStats.h"
#include "naucrates/md/CDXLRelStats.h"
#include "gpopt/operators/CScalarArray.h"
#include "gpopt/xforms/CXform.h"
#include "gpopt/mdcache/CMDAccessor.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CHistogram.h"
#include "gpopt/operators/CExpressionUtils.h"
#include "gpopt/operators/CPhysical.h"
// DXL stubs
namespace gpdxl {
const CWStringBase* GetDXLTokenStr(int) { return NULL; }
CWStringDynamic* SerializeMDObj(CMemoryPool*, const IMDCacheObject*, bool, bool) { return NULL; }
CWStringBase* Read(CMemoryPool*, const CHAR*) { return NULL; }
IMDCacheObjectArray* ParseDXLToIMDObjectArray(CMemoryPool*, const CHAR*, const CHAR*) { return NULL; }
}
void gpdxl::CXMLSerializer::OpenElement(const CWStringBase*, const CWStringBase*) {}
void gpdxl::CXMLSerializer::CloseElement(const CWStringBase*, const CWStringBase*) {}
void gpdxl::CXMLSerializer::AddAttribute(const CWStringBase*, BOOL) {}
void gpdxl::CDXLNode::SerializeToDXL(CXMLSerializer*) const {}
// Physical destructors
gpopt::CPhysicalSort::~CPhysicalSort() {}
gpopt::CPhysicalMotionGather::~CPhysicalMotionGather() {}
gpopt::CPhysicalMotionRoutedDistribute::~CPhysicalMotionRoutedDistribute() {}
gpopt::CPhysicalSpool::~CPhysicalSpool() {}
gpopt::CPhysicalPartitionSelector::~CPhysicalPartitionSelector() {}
gpopt::CPhysicalDynamicScan::~CPhysicalDynamicScan() {}
gpopt::CPhysicalUnionAll::~CPhysicalUnionAll() {}
// Logical destructors
gpopt::CLogicalSelect::~CLogicalSelect() {}
gpopt::CLogicalGbAgg::~CLogicalGbAgg() {}
gpopt::CLogicalSequenceProject::~CLogicalSequenceProject() {}
// Method stubs
void gpopt::CPhysical::LookupRequest(ULONG, ULONG*, ULONG*, ULONG*, ULONG*) {}
bool gpopt::CPhysicalUnionAll::IsPartialIndex() const { return false; }
gpopt::ULONG gpopt::CPhysicalUnionAll::UlScanIdPartialIndex() const { return 0; }
bool gpopt::CScalarIdent::FCastedScId(gpopt::CExpression* p) { (void)p; return false; }
bool gpopt::CScalarAggFunc::FCountStar() const { return false; }
bool gpopt::CScalarAggFunc::FCountAny() const { return false; }
bool gpopt::CScalarProjectList::FHasMultipleDistinctAggs(gpopt::CExpressionHandle&) { return false; }
bool gpopt::CScalarProjectList::FHasScalarFunc(gpopt::CExpressionHandle&) { return false; }
gpopt::ULONG gpopt::CScalarProjectList::UlDistinctAggs(gpopt::CExpressionHandle&) { return 0; }
gpopt::ULONG gpopt::CScalarProjectList::UlOrderedAggs(gpopt::CExpressionHandle&) { return 0; }
gpopt::CScalarConstArray* gpopt::CScalarArray::PdrgPconst() const { return NULL; }
gpopt::CExpression* gpopt::CExpressionUtils::PexprDedupChildren(gpopt::CMemoryPool* mp, gpopt::CExpression* p) { (void)mp; return p; }
bool gpopt::CXform::FEqualIds(const CHAR* a, const CHAR* b) { return strcmp(a, b) == 0; }
gpopt::CExpression* gpopt::CXformResult::PexprNext() { return NULL; }
const gpopt::IMDType* gpopt::CMDAccessor::RetrieveType(gpmd::IMDType::ETypeInfo ti) { (void)ti; return NULL; }
const gpmd::IMDRelation* gpopt::CMDAccessor::RetrieveRel(gpmd::IMDId*) { return NULL; }
gpnaucrates::IStatistics* gpopt::CMDAccessor::Pstats(CMemoryPool*, IMDId*, CColRefSet*, CColRefSet*, CStatisticsConfig*) { return NULL; }
gpnaucrates::CDXLStatsDerivedRelation* gpnaucrates::CStatistics::GetDxlStatsDrvdRelation(CMemoryPool*, gpopt::CMDAccessor*) const { return NULL; }
gpmd::CDXLColStats* gpmd::CDXLColStats::CreateDXLDummyColStats(CMemoryPool*, IMDId*, CMDName*, CDouble) { return NULL; }
gpmd::CDXLRelStats* gpmd::CDXLRelStats::CreateDXLDummyRelStats(CMemoryPool*, IMDId*) { return NULL; }
// Inline stubs for missing classes
namespace gpopt {
class CScalarOp : public CScalar {
public:
    CScalarOp(CMemoryPool* mp, gpmd::IMDId*, gpmd::IMDId*, const CWStringConst*) : CScalar(mp) {}
    gpmd::IMDId* MdIdOp() const { return NULL; }
};
class CScalarFunc : public CScalar {
public:
    CScalarFunc(CMemoryPool* mp) : CScalar(mp) {}
    gpmd::IMDId* FuncMdId() const { return NULL; }
};
class CPhysicalJoin : public CPhysical {
public:
    CPhysicalJoin(CMemoryPool* mp) : CPhysical(mp) {}
    static BOOL FHashJoinCompatible(CExpression*, CExpression*, CExpression*) { return false; }
};
}



// Virtual functions for CLogicalSequenceProject vtable
gpopt::CColRefSet* gpopt::CLogicalSequenceProject::DeriveOutputColumns(CMemoryPool* mp, CExpressionHandle&) { return GPOS_NEW(mp) CColRefSet(mp); }
gpopt::CColRefSet* gpopt::CLogicalSequenceProject::DeriveOuterReferences(CMemoryPool* mp, CExpressionHandle&) { return GPOS_NEW(mp) CColRefSet(mp); }
gpopt::CKeyCollection* gpopt::CLogicalSequenceProject::DeriveKeyCollection(CMemoryPool* mp, CExpressionHandle&) const { (void)mp; return NULL; }
gpopt::CMaxCard gpopt::CLogicalSequenceProject::DeriveMaxCard(CMemoryPool* mp, CExpressionHandle&) const { (void)mp; return CMaxCard(); }
gpnaucrates::IStatistics* gpopt::CLogicalSequenceProject::PstatsDerive(CMemoryPool* mp, CExpressionHandle&, gpnaucrates::IStatisticsArray*) const { (void)mp; return NULL; }
gpopt::CXformSet* gpopt::CLogicalSequenceProject::PxfsCandidates(CMemoryPool* mp) const { return GPOS_NEW(mp) CXformSet(mp); }


// Also add CLogicalUnary virtual functions that are pure


// Additional vtable stubs for logical operators
gpopt::IOstream& gpopt::CLogicalSequenceProject::OsPrint(IOstream& os) const { return os; }
gpopt::ULONG gpopt::CLogicalSequenceProject::HashValue() const { return 0; }
bool gpopt::CLogicalSequenceProject::Matches(gpopt::COperator*) const { return false; }
gpopt::COperator* gpopt::CLogicalSequenceProject::PopCopyWithRemappedColumns(CMemoryPool*, UlongToColRefMap*, bool) { return NULL; }

// CLogicalGbAgg vtable stubs
gpopt::IOstream& gpopt::CLogicalGbAgg::OsPrint(IOstream& os) const { return os; }
gpopt::CXformSet* gpopt::CLogicalGbAgg::PxfsCandidates(CMemoryPool* mp) const { return GPOS_NEW(mp) CXformSet(mp); }
gpnaucrates::IStatistics* gpopt::CLogicalGbAgg::PstatsDerive(CMemoryPool* mp, CExpressionHandle&, gpnaucrates::IStatisticsArray*) const { (void)mp; return NULL; }
gpopt::CColRefSet* gpopt::CLogicalGbAgg::PcrsStat(CMemoryPool* mp, CExpressionHandle&, CColRefSet*, ULONG) const { (void)mp; return NULL; }

// CLogicalUnary vtable - remove the forced vtable emission by removing destructor


// CLogicalUnary pure virtual definitions (needed for vtable emission)

// CLogicalGbAgg stubs


// CLogical pure virtual definitions (needed by CLogicalUnary and CLogicalGbAgg vtables)
gpopt::CColRefSet* gpopt::CLogical::DeriveOutputColumns(CMemoryPool* mp, CExpressionHandle&) { (void)mp; return NULL; }
gpopt::CPropConstraint* gpopt::CLogical::DerivePropertyConstraint(CMemoryPool* mp, CExpressionHandle&) const { (void)mp; return NULL; }
gpnaucrates::IStatistics* gpopt::CLogical::PstatsDerive(CMemoryPool* mp, CExpressionHandle&, gpnaucrates::IStatisticsArray*) const { (void)mp; return NULL; }
gpopt::CXformSet* gpopt::CLogical::PxfsCandidates(CMemoryPool* mp) const { (void)mp; return NULL; }
