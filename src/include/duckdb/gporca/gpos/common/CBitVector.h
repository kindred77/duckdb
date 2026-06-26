//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CBitVector.h
//
//	@doc:
//		Implementation of static bit vector;
//---------------------------------------------------------------------------
#ifndef GPOS_CBitVector_H
#define GPOS_CBitVector_H

#include "gpos/base.h"

namespace gpos
{
//---------------------------------------------------------------------------
//	@class:
//		CBitVector
//
//	@doc:
//		Bit vector based on ULLONG elements
//
//---------------------------------------------------------------------------
class CBitVector
{
private:
	// size in bits
	GP_ULONG m_nbits;

	// size of vector in units, not bits
	GP_ULONG m_len;

	// vector
	ULLONG *m_vec;

	// no default copy ctor
	CBitVector(const CBitVector &);

	// clear vector
	void Clear();

public:
	// ctor
	CBitVector(CMemoryPool *mp, GP_ULONG cBits);

	// dtor
	~CBitVector();

	// copy ctor with target mem pool
	CBitVector(CMemoryPool *mp, const CBitVector &);

	// determine if bit is set
	GP_BOOL Get(GP_ULONG ulBit) const;

	// set given bit; return previous value
	GP_BOOL ExchangeSet(GP_ULONG ulBit);

	// clear given bit; return previous value
	GP_BOOL ExchangeClear(GP_ULONG ulBit);

	// union vectors
	void Or(const CBitVector *);

	// intersect vectors
	void And(const CBitVector *);

	// is subset
	GP_BOOL ContainsAll(const CBitVector *) const;

	// is dijoint
	GP_BOOL IsDisjoint(const CBitVector *) const;

	// equality
	GP_BOOL Equals(const CBitVector *) const;

	// is empty?
	GP_BOOL IsEmpty() const;

	// find next bit from given position
	GP_BOOL GetNextSetBit(GP_ULONG, GP_ULONG &) const;

	// number of bits set
	GP_ULONG CountSetBits() const;

	// hash value
	GP_ULONG HashValue() const;

};	// class CBitVector

}  // namespace gpos

#endif	// !GPOS_CBitVector_H

// EOF
