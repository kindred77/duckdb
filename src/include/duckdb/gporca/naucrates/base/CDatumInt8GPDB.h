//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDatumInt8GPDB.h
//
//	@doc:
//		GPDB-specific Int8 representation
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CDatumInt8GPDB_H
#define GPNAUCRATES_CDatumInt8GPDB_H

#include "gpos/base.h"

#include "naucrates/base/IDatumInt8.h"
#include "naucrates/md/CMDTypeInt8GPDB.h"

namespace gpnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumInt8GPDB
//
//	@doc:
//		GPDB-specific Int8 representation
//
//---------------------------------------------------------------------------
class CDatumInt8GPDB : public IDatumInt8
{
private:
	// type information
	IMDId *m_mdid;

	// integer value
	LINT m_val;

	// is null
	GP_BOOL m_is_null;


	// private copy ctor
	CDatumInt8GPDB(const CDatumInt8GPDB &);

public:
	// ctors
	CDatumInt8GPDB(CSystemId sysid, LINT val, GP_BOOL is_null = false);
	CDatumInt8GPDB(IMDId *mdid, LINT val, GP_BOOL is_null = false);

	// dtor
	virtual ~CDatumInt8GPDB();

	// accessor of metadata type id
	virtual IMDId *MDId() const;

	// accessor of size
	virtual GP_ULONG Size() const;

	// accessor of integer value
	virtual LINT Value() const;

	// accessor of is null
	virtual GP_BOOL IsNull() const;

	// return string representation
	virtual const CWStringConst *GetStrRepr(CMemoryPool *mp) const;

	// hash function
	virtual GP_ULONG HashValue() const;

	// match function for datums
	virtual GP_BOOL Matches(const IDatum *) const;

	// copy datum
	virtual IDatum *MakeCopy(CMemoryPool *mp) const;

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CDatumInt8GPDB

}  // namespace gpnaucrates


#endif	// !GPNAUCRATES_CDatumInt8GPDB_H

// EOF
