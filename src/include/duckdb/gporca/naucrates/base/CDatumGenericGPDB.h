//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumGenericGPDB.h
//
//	@doc:
//		GPDB-specific generic datum representation
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CDatumGenericGPDB_H
#define GPNAUCRATES_CDatumGenericGPDB_H

#include "gpos/base.h"

#include "naucrates/base/IDatumGeneric.h"
#include "naucrates/md/CMDTypeGenericGPDB.h"

#define GPDB_DATUM_HDRSZ 4

namespace gpnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumGenericGPDB
//
//	@doc:
//		GPDB-specific generic datum representation
//
//---------------------------------------------------------------------------
class CDatumGenericGPDB : public IDatumGeneric
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// size in bytes
	GP_ULONG m_size;

	// a pointer to datum value
	BYTE *m_bytearray_value;

	// is null
	GP_BOOL m_is_null;

	// type information
	IMDId *m_mdid;

	INT m_type_modifier;

	// cached type information (can be set from const methods)
	mutable const IMDType *m_cached_type;

	// long int value used for statistic computation
	LINT m_stats_comp_val_int;

	// double value used for statistic computation
	CDouble m_stats_comp_val_double;

	// private copy ctor
	CDatumGenericGPDB(const CDatumGenericGPDB &);

public:
	// ctor
	CDatumGenericGPDB(CMemoryPool *mp, IMDId *mdid, INT type_modifier,
					  const void *src, GP_ULONG size, GP_BOOL is_null,
					  LINT stats_comp_val_int, CDouble stats_comp_val_double);

	// dtor
	virtual ~CDatumGenericGPDB();

	// accessor of metadata type id
	virtual IMDId *MDId() const;

	virtual INT TypeModifier() const;

	// accessor of size
	virtual GP_ULONG Size() const;

	// accessor of is null
	virtual GP_BOOL IsNull() const;

	// return string representation
	virtual const CWStringConst *GetStrRepr(CMemoryPool *mp) const;

	// hash function
	virtual GP_ULONG HashValue() const;

	// match function for datums
	virtual GP_BOOL Matches(const IDatum *datum) const;

	// copy datum
	virtual IDatum *MakeCopy(CMemoryPool *mp) const;

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

	// accessor to bytearray, creates a copy
	virtual BYTE *MakeCopyOfValue(CMemoryPool *mp, GP_ULONG *pulLength) const;

	// statistics related APIs

	// can datum be mapped to a double
	virtual GP_BOOL IsDatumMappableToDouble() const;

	// map to double for stats computation
	virtual CDouble
	GetDoubleMapping() const
	{
		GPOS_ASSERT(IsDatumMappableToDouble());

		return m_stats_comp_val_double;
	}

	// can datum be mapped to LINT
	virtual GP_BOOL IsDatumMappableToLINT() const;

	// map to LINT for statistics computation
	virtual LINT
	GetLINTMapping() const
	{
		GPOS_ASSERT(IsDatumMappableToLINT());

		return m_stats_comp_val_int;
	}

	// byte array representation of datum
	virtual const BYTE *GetByteArrayValue() const;

	// stats equality
	virtual GP_BOOL StatsAreEqual(const IDatum *datum) const;

	// does the datum need to be padded before statistical derivation
	virtual GP_BOOL NeedsPadding() const;

	// return the padded datum
	virtual IDatum *MakePaddedDatum(CMemoryPool *mp, GP_ULONG col_len) const;

	// does datum support like predicate
	virtual GP_BOOL
	SupportsLikePredicate() const
	{
		return true;
	}

	// return the default scale factor of like predicate
	virtual CDouble GetLikePredicateScaleFactor() const;

	// default selectivity of the trailing wildcards
	virtual CDouble GetTrailingWildcardSelectivity(const BYTE *pba,
												   GP_ULONG ulPos) const;

	// selectivities needed for LIKE predicate statistics evaluation
	static const CDouble DefaultFixedCharSelectivity;
	static const CDouble DefaultCharRangeSelectivity;
	static const CDouble DefaultAnyCharSelectivity;
	static const CDouble DefaultCdbRanchorSelectivity;
	static const CDouble DefaultCdbRolloffSelectivity;

};	// class CDatumGenericGPDB
}  // namespace gpnaucrates


#endif	// !GPNAUCRATES_CDatumGenericGPDB_H

// EOF
