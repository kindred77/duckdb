#include "gpopt/operators/CScalarFunc.h"
gpopt::CScalarFunc::CScalarFunc(CMemoryPool* mp, IMDId*, IMDId*, INT mod, const CWStringConst*) : CScalar(mp), m_return_type_modifier(mod) {}
gpmd::IMDId* gpopt::CScalarFunc::FuncMdId() const { return NULL; }
