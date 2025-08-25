#ifndef GPOPT_CUTIL_H
#define GPOPT_CUTIL_H

#include "gpos/base.h"
#include "gpos/common/CAutoP.h"
#include "gpos/io/IOstream.h"
#include "gpos/string/CWStringDynamic.h"

#include "gpopt/metadata/CName.h"

#include "naucrates/md/IMDType.h"

#include "duckdb/common/types.hpp"

namespace gpopt
{

class CUtil
{
public:
	// create a GPOS dynamic string from a regular character array
	static gpos::CWStringDynamic *CreateDynamicStringFromCharArray(gpos::CMemoryPool *mp,
	                                                               const gpos::CHAR *c);

	static gpopt::CName *CreateNameFromCharArray(gpos::CMemoryPool *mp,
	                                                               const gpos::CHAR *c);

	static const gpmd::IMDType *GetMdType(gpos::CMemoryPool *mp, const duckdb::LogicalType &type);
};

}

#endif // GPOPT_CUTIL_H
