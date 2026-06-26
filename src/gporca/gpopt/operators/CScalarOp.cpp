#include "gpopt/operators/CScalarOp.h"
gpopt::CScalarOp::CScalarOp(CMemoryPool* mp, IMDId*, IMDId*, const CWStringConst*) : CScalar(mp) {}
gpmd::IMDId* gpopt::CScalarOp::MdIdOp() const { return NULL; }
