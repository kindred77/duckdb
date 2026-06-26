#include "gpopt/operators/CPhysicalJoin.h"
gpopt::CPhysicalJoin::CPhysicalJoin(CMemoryPool* mp, CXform::EXformId) : CPhysical(mp) {}
BOOL gpopt::CPhysicalJoin::FHashJoinCompatible(CExpression*, CExpression*, CExpression*) { return false; }
