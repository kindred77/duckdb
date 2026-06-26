//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDTrigger.h
//
//	@doc:
//		Interface for triggers in the metadata cache
//---------------------------------------------------------------------------

#ifndef GPMD_IMDTrigger_H
#define GPMD_IMDTrigger_H

#include "gpos/base.h"

#include "naucrates/md/IMDCacheObject.h"

namespace gpmd
{
using namespace gpos;


//---------------------------------------------------------------------------
//	@class:
//		IMDTrigger
//
//	@doc:
//		Interface for triggers in the metadata cache
//
//---------------------------------------------------------------------------
class IMDTrigger : public IMDCacheObject
{
public:
	// object type
	virtual Emdtype
	MDType() const
	{
		return EmdtTrigger;
	}

	// does trigger execute on a row-level
	virtual GP_BOOL ExecutesOnRowLevel() const = 0;

	// is this a before trigger
	virtual GP_BOOL IsBefore() const = 0;

	// is this an insert trigger
	virtual GP_BOOL IsInsert() const = 0;

	// is this a delete trigger
	virtual GP_BOOL IsDelete() const = 0;

	// is this an update trigger
	virtual GP_BOOL IsUpdate() const = 0;

	// relation mdid
	virtual IMDId *GetRelMdId() const = 0;

	// function mdid
	virtual IMDId *FuncMdId() const = 0;

	// is trigger enabled
	virtual GP_BOOL IsEnabled() const = 0;
};
}  // namespace gpmd

#endif	// !GPMD_IMDTrigger_H

// EOF
