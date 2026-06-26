#include "gpopt/operators/CLogicalSelect.h"
gpopt::CLogicalSelect::CLogicalSelect(CMemoryPool* mp) : CLogicalUnary(mp) {}
gpopt::CLogicalSelect::CLogicalSelect(CMemoryPool* mp, CTableDescriptor* t) : CLogicalUnary(mp) { (void)t; }
