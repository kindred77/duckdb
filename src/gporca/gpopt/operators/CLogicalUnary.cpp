#include "gpopt/operators/CLogicalUnary.h"
gpopt::CLogical::EStatPromise gpopt::CLogicalUnary::Esp(CExpressionHandle&) const { return EspLow; }
