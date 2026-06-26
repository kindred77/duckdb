#include "gpopt/operators/CPhysicalSort.h"
gpopt::CPhysicalSort::CPhysicalSort(CMemoryPool* mp, COrderSpec* pos) : CPhysical(mp) { (void)pos; }
gpopt::CPhysicalSort::~CPhysicalSort() {}
