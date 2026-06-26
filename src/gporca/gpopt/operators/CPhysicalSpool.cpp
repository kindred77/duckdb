#include "gpopt/operators/CPhysicalSpool.h"
gpopt::CPhysicalSpool::CPhysicalSpool(CMemoryPool* mp, bool b) : CPhysical(mp) { (void)b; }
gpopt::CPhysicalSpool::~CPhysicalSpool() {}
