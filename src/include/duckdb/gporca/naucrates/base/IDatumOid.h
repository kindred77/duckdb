//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IDatumOid.h
//
//	@doc:
//		Base abstract class for oid representation
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_IDatumOid_H
#define GPNAUCRATES_IDatumOid_H

#include "gpos/base.h"

#include "naucrates/base/IDatum.h"

namespace gpnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		IDatumOid
//
//	@doc:
//		Base abstract class for oid representation
//
//---------------------------------------------------------------------------
class IDatumOid : public IDatum
{
private:
	// private copy ctor
	IDatumOid(const IDatumOid &);

public:
	// ctor
	IDatumOid(){};

	// dtor
	virtual ~IDatumOid(){};

	// accessor for datum type
	virtual IMDType::ETypeInfo
	GetDatumType()
	{
		return IMDType::EtiOid;
	}

	// accessor of oid value
	virtual OID OidValue() const = 0;

	// can datum be mapped to a double
	GP_BOOL
	IsDatumMappableToDouble() const
	{
		return true;
	}

	// map to double for stats computation
	CDouble
	GetDoubleMapping() const
	{
		return CDouble(GetLINTMapping());
	}

	// can datum be mapped to LINT
	GP_BOOL
	IsDatumMappableToLINT() const
	{
		return true;
	}

	// map to LINT for statistics computation
	LINT
	GetLINTMapping() const
	{
		return LINT(OidValue());
	}

	// byte array representation of datum
	virtual const BYTE *
	GetByteArrayValue() const
	{
		GPOS_ASSERT(!"Invalid invocation of MakeCopyOfValue");
		return NULL;
	}

	// does the datum need to be padded before statistical derivation
	virtual GP_BOOL
	NeedsPadding() const
	{
		return false;
	}

	// return the padded datum
	virtual IDatum *
	MakePaddedDatum(CMemoryPool *,	// mp,
					GP_ULONG			// col_len
	) const
	{
		GPOS_ASSERT(!"Invalid invocation of MakePaddedDatum");
		return NULL;
	}

	// does datum support like predicate
	virtual GP_BOOL
	SupportsLikePredicate() const
	{
		return false;
	}

	// return the default scale factor of like predicate
	virtual CDouble
	GetLikePredicateScaleFactor() const
	{
		GPOS_ASSERT(!"Invalid invocation of DLikeSelectivity");
		return false;
	}
};	// class IDatumOid
}  // namespace gpnaucrates

#endif	// !GPNAUCRATES_IDatumOid_H

// EOF
