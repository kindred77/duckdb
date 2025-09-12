#include "gpopt/translate/CUtil.h"

#include "gpopt/exception.h"

#include "naucrates/md/CMDTypeBoolGPDB.h"
#include "naucrates/md/CMDTypeInt4GPDB.h"
#include "naucrates/md/CMDTypeInt8GPDB.h"

using namespace gpmd;
using namespace gpos;
using namespace duckdb;

CWStringDynamic *
CUtil::CreateDynamicStringFromCharArray(CMemoryPool *mp, const CHAR *c)
{
	GPOS_ASSERT(NULL != c);

	CAutoP<CWStringDynamic> string_var(GPOS_NEW(mp) CWStringDynamic(mp));
	string_var->AppendFormat(GPOS_WSZ_LIT("%s"), c);
	return string_var.Reset();
}

CName *
CUtil::CreateNameFromCharArray(CMemoryPool *mp,
                               const CHAR *c)
{
	CWStringDynamic *str_name =
	    CreateDynamicStringFromCharArray(mp, c);
	CWStringConst *string_const = GPOS_NEW(mp) CWStringConst(mp, str_name->GetBuffer());
	return GPOS_NEW(mp) CName(mp, string_const);
}

const IMDType *
CUtil::GetMdType(CMemoryPool *mp, const LogicalType &type)
{
	switch(type.id())
	{
	case LogicalTypeId::BOOLEAN:
		return GPOS_NEW(mp) CMDTypeBoolGPDB(mp);
	case LogicalTypeId::INTEGER:
		return GPOS_NEW(mp) CMDTypeInt4GPDB(mp);
	case LogicalTypeId::BIGINT:
	case LogicalTypeId::HUGEINT:
		return GPOS_NEW(mp) CMDTypeInt8GPDB(mp);
	case LogicalTypeId::DATE:
		return GPOS_NEW(mp) CMDTypeInt8GPDB(mp);
	default:
		GPOS_RAISE(gpopt::ExmaGPOPT, gpopt::ExmiUnsupportedOp,
		           type.ToString().c_str());
	}
}