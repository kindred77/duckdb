#include "gpopt/operators/CLogicalGbAgg.h"
gpopt::CLogicalGbAgg::CLogicalGbAgg(CMemoryPool* mp, CColRefArray* c, COperator::EGbAggType t) : CLogicalUnary(mp) { (void)c;(void)t; }
