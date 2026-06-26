#include "gpopt/operators/CPhysicalPartitionSelector.h"
gpopt::CPhysicalPartitionSelector::CPhysicalPartitionSelector(CMemoryPool* mp, GP_ULONG scan_id, IMDId* mdid, CColRef2dArray* pk, UlongToPartConstraintMap* pc, CPartConstraint* cn, UlongToExprMap* fc, UlongToExprMap* pfc, CExpression* rf)
    : CPhysical(mp) { (void)scan_id;(void)mdid;(void)pk;(void)pc;(void)cn;(void)fc;(void)pfc;(void)rf; }
gpopt::CPhysicalPartitionSelector::~CPhysicalPartitionSelector() {}
