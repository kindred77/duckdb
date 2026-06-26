//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CBitSet.h
//
//	@doc:
//		Implementation of bitset as linked list of bitvectors
//---------------------------------------------------------------------------
#ifndef GPOS_CBitSet_H
#define GPOS_CBitSet_H

#include "gpos/base.h"
#include "gpos/common/CBitVector.h"
#include "gpos/common/CDynamicPtrArray.h"
#include "gpos/common/CList.h"
#include "gpos/common/DbgPrintMixin.h"


namespace gpos
{
//---------------------------------------------------------------------------
//	@class:
//		CBitSet
//
//	@doc:
//		Linked list of CBitSetLink's
//
//---------------------------------------------------------------------------
class CBitSet : public CRefCount, public DbgPrintMixin<CBitSet>
{
	// bitset iter needs to access internals
	friend class CBitSetIter;

protected:
	//---------------------------------------------------------------------------
	//	@class:
	//		CBitSetLink
	//
	//	@doc:
	//		bit vector + offset + link
	//
	//---------------------------------------------------------------------------
	class CBitSetLink
	{
	private:
		// private copy ctor
		CBitSetLink(const CBitSetLink &);

		// offset
		GP_ULONG m_offset;

		// bitvector
		CBitVector *m_vec;

	public:
		// ctor
		explicit CBitSetLink(CMemoryPool *, GP_ULONG offset, GP_ULONG vector_size);

		explicit CBitSetLink(CMemoryPool *, const CBitSetLink &);

		// dtor
		~CBitSetLink();

		// accessor
		GP_ULONG
		GetOffset() const
		{
			return m_offset;
		}

		// accessor
		CBitVector *
		GetVec() const
		{
			return m_vec;
		}

		// list link
		SLink m_link;

	};	// class CBitSetLink

	// list of bit set links
	CList<CBitSetLink> m_bsllist;

	// pool to allocate links from
	CMemoryPool *m_mp;

	// size of individual bitvectors
	GP_ULONG m_vector_size;

	// number of elements
	GP_ULONG m_size;

	// private copy ctor
	CBitSet(const CBitSet &);

	// find link with offset less or equal to given value
	CBitSetLink *FindLinkByOffset(GP_ULONG, CBitSetLink * = NULL) const;

	// reset set
	void Clear();

	// compute target offset
	GP_ULONG ComputeOffset(GP_ULONG) const;

	// re-compute size of set
	void RecomputeSize();

public:
	// ctor
	CBitSet(CMemoryPool *mp, GP_ULONG vector_size = 256);
	CBitSet(CMemoryPool *mp, const CBitSet &);

	// dtor
	virtual ~CBitSet();

	// determine if bit is set
	GP_BOOL Get(GP_ULONG pos) const;

	// set given bit; return previous value
	GP_BOOL ExchangeSet(GP_ULONG pos);

	// clear given bit; return previous value
	GP_BOOL ExchangeClear(GP_ULONG pos);

	// union sets
	void Union(const CBitSet *);

	// intersect sets
	void Intersection(const CBitSet *);

	// difference of sets
	void Difference(const CBitSet *);

	// is subset
	GP_BOOL ContainsAll(const CBitSet *) const;

	// equality
	GP_BOOL Equals(const CBitSet *) const;

	// disjoint
	GP_BOOL IsDisjoint(const CBitSet *) const;

	// hash value for set
	GP_ULONG HashValue() const;

	// number of elements
	GP_ULONG
	Size() const
	{
		return m_size;
	}

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CBitSet


// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CBitSet &bs)
{
	return bs.OsPrint(os);
}
}  // namespace gpos

#endif	// !GPOS_CBitSet_H

// EOF
