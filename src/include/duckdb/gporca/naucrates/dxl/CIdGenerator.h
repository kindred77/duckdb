//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CIdGenerator.h
//
//	@doc:
//		Class providing methods for a GP_ULONG counter
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef GPDXL_CIdGenerator_H
#define GPDXL_CIdGenerator_H

#define GPDXL_INVALID_ID gpos::ulong_max

#include "gpos/base.h"

namespace gpdxl
{
using namespace gpos;

class CIdGenerator
{
private:
	GP_ULONG id;

public:
	explicit CIdGenerator(GP_ULONG);
	GP_ULONG next_id();
	GP_ULONG current_id();
};
}  // namespace gpdxl
#endif	// GPDXL_CIdGenerator_H

// EOF
