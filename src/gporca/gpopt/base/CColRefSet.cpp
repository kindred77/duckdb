//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSet.cpp
//
//	@doc:
//		Implementation of column reference set based on bit sets
//---------------------------------------------------------------------------

#include "gpopt/base/CColRefSet.h"

#include "gpos/base.h"

#include "gpopt/base/CAutoOptCtxt.h"
#include "gpopt/base/CColRefSetIter.h"
#include "gpopt/base/CColumnFactory.h"

using namespace gpopt;

FORCE_GENERATE_DBGSTR(CColRefSet);

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::CColRefSet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CColRefSet::CColRefSet(CMemoryPool *mp, GP_ULONG ulSizeBits)
	: CBitSet(mp, ulSizeBits)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::CColRefSet
//
//	@doc:
//		copy ctor;
//
//---------------------------------------------------------------------------
CColRefSet::CColRefSet(CMemoryPool *mp, const CColRefSet &bs) : CBitSet(mp, bs)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::CColRefSet
//
//	@doc:
//		ctor, copy from col refs array
//
//---------------------------------------------------------------------------
CColRefSet::CColRefSet(CMemoryPool *mp, const CColRefArray *colref_array,
					   GP_ULONG size)
	: CBitSet(mp, size)
{
	Include(colref_array);
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::~CColRefSet
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CColRefSet::~CColRefSet()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::FMember
//
//	@doc:
//		Check if given column ref is in the set
//
//---------------------------------------------------------------------------
GP_BOOL
CColRefSet::FMember(const CColRef *colref) const
{
	return CBitSet::Get(colref->Id());
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::PcrAny
//
//	@doc:
//		Return random member
//
//---------------------------------------------------------------------------
CColRef *
CColRefSet::PcrAny() const
{
	// for now return the first column
	return PcrFirst();
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::PcrFirst
//
//	@doc:
//		Return first member
//
//---------------------------------------------------------------------------
CColRef *
CColRefSet::PcrFirst() const
{
	CColRefSetIter crsi(*this);
	if (crsi.Advance())
	{
		return crsi.Pcr();
	}

	GPOS_ASSERT(0 == Size());
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Include
//
//	@doc:
//		Include a constant column ref in set
//
//---------------------------------------------------------------------------
void
CColRefSet::Include(const CColRef *colref)
{
	CBitSet::ExchangeSet(colref->Id());
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Include
//
//	@doc:
//		Include column refs from an array
//
//---------------------------------------------------------------------------
void
CColRefSet::Include(const CColRefArray *colref_array)
{
	GP_ULONG length = colref_array->Size();
	for (GP_ULONG i = 0; i < length; i++)
	{
		Include((*colref_array)[i]);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Include
//
//	@doc:
//		Include a set of columns in bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Include(const CColRefSet *pcrs)
{
	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		Include(crsi.Pcr());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Exclude
//
//	@doc:
//		Remove column from bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Exclude(const CColRef *colref)
{
	CBitSet::ExchangeClear(colref->Id());
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Exclude
//
//	@doc:
//		Remove a set of columns from bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Exclude(const CColRefSet *pcrs)
{
	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		Exclude(crsi.Pcr());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Exclude
//
//	@doc:
//		Remove an array of columns from bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Exclude(const CColRefArray *colref_array)
{
	for (GP_ULONG i = 0; i < colref_array->Size(); i++)
	{
		Exclude((*colref_array)[i]);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Replace
//
//	@doc:
//		Replace column with another column in bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Replace(const CColRef *pcrOut, const CColRef *pcrIn)
{
	if (FMember(pcrOut))
	{
		Exclude(pcrOut);
		Include(pcrIn);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Replace
//
//	@doc:
//		Replace an array of columns with another array of columns
//
//---------------------------------------------------------------------------
void
CColRefSet::Replace(const CColRefArray *pdrgpcrOut,
					const CColRefArray *pdrgpcrIn)
{
	const GP_ULONG length = pdrgpcrOut->Size();
	GPOS_ASSERT(length == pdrgpcrIn->Size());

	for (GP_ULONG ul = 0; ul < length; ul++)
	{
		Replace((*pdrgpcrOut)[ul], (*pdrgpcrIn)[ul]);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Pdrgpcr
//
//	@doc:
//		Convert set into array
//
//---------------------------------------------------------------------------
CColRefArray *
CColRefSet::Pdrgpcr(CMemoryPool *mp) const
{
	CColRefArray *colref_array = GPOS_NEW(mp) CColRefArray(mp);

	CColRefSetIter crsi(*this);
	while (crsi.Advance())
	{
		colref_array->Append(crsi.Pcr());
	}

	return colref_array;
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Phmicr
//
//	@doc:
//		Convert set into map
//
//---------------------------------------------------------------------------
IntToColRefMap *
CColRefSet::Phmicr(CMemoryPool *mp) const
{
	IntToColRefMap *phmicr = GPOS_NEW(mp) IntToColRefMap(mp);

	CColRefSetIter crsi(*this);
	while (crsi.Advance())
	{
		phmicr->Insert(GPOS_NEW(mp) INT(crsi.Pcr()->Id()), crsi.Pcr());
	}

	return phmicr;
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::HashValue
//
//	@doc:
//		Compute hash value by combining hashes of components
//
//---------------------------------------------------------------------------
GP_ULONG
CColRefSet::HashValue()
{
	GP_ULONG size = this->Size();
	GP_ULONG ulHash = gpos::HashValue<GP_ULONG>(&size);

	// limit the number of columns used in hash computation
	GP_ULONG length = std::min(size, (GP_ULONG) 8);

	CColRefSetIter crsi(*this);
	for (GP_ULONG i = 0; i < length; i++)
	{
		(void) crsi.Advance();
		ulHash =
			gpos::CombineHashes(ulHash, gpos::HashPtr<CColRef>(crsi.Pcr()));
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::OsPrint
//
//	@doc:
//		Helper function to print a colref set
//
//---------------------------------------------------------------------------
IOstream &
CColRefSet::OsPrint(IOstream &os) const
{
	return OsPrint(os, gpos::ulong_max);
}

IOstream &
CColRefSet::OsPrint(IOstream &os, GP_ULONG ulLenMax) const
{
	GP_ULONG length = Size();
	GP_ULONG ul = 0;

	CColRefSetIter crsi(*this);
	while (crsi.Advance() && ul < std::min(length, ulLenMax))
	{
		CColRef *colref = crsi.Pcr();
		colref->OsPrint(os);
		if (ul < length - 1)
		{
			os << ", ";
		}
		ul++;
	}

	if (ulLenMax < length)
	{
		os << "...";
	}

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::ExtractColIds
//
//	@doc:
//		Extract array of column ids from colrefset
//
//---------------------------------------------------------------------------
void
CColRefSet::ExtractColIds(CMemoryPool *mp, ULongPtrArray *colids) const
{
	CColRefSetIter crsi(*this);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		GP_ULONG colid = colref->Id();
		colids->Append(GPOS_NEW(mp) GP_ULONG(colid));
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::FContained
//
//	@doc:
//		Check if the current colrefset is a subset of any of the colrefsets in
//		the given array
//
//---------------------------------------------------------------------------
GP_BOOL
CColRefSet::FContained(const CColRefSetArray *pdrgpcrs)
{
	GPOS_ASSERT(NULL != pdrgpcrs);

	const GP_ULONG length = pdrgpcrs->Size();
	for (GP_ULONG ul = 0; ul < length; ul++)
	{
		if ((*pdrgpcrs)[ul]->ContainsAll(this))
		{
			return true;
		}
	}

	return false;
}

GP_BOOL
CColRefSet::FIntersects(const CColRefSet *pcrs)
{
	GPOS_ASSERT(NULL != pcrs);
	CColRefSet *intersecting_colrefset = GPOS_NEW(m_mp) CColRefSet(m_mp, *this);
	intersecting_colrefset->Intersection(pcrs);

	GP_BOOL intersects = intersecting_colrefset->Size() > 0;
	intersecting_colrefset->Release();

	return intersects;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::FCovered
//
//	@doc:
//		Are the columns in the column reference set covered by the array of
//		column ref sets
//---------------------------------------------------------------------------
GP_BOOL
CColRefSet::FCovered(CColRefSetArray *pdrgpcrs, CColRefSet *pcrs)
{
	GPOS_ASSERT(NULL != pdrgpcrs);
	GPOS_ASSERT(NULL != pcrs);
	GPOS_ASSERT(0 < pdrgpcrs->Size());

	if (0 == pcrs->Size())
	{
		return false;
	}

	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		GP_BOOL fFound = false;
		const GP_ULONG length = pdrgpcrs->Size();
		for (GP_ULONG ul = 0; ul < length && !fFound; ul++)
		{
			CColRefSet *pcrs = (*pdrgpcrs)[ul];
			if (pcrs->FMember(colref))
			{
				fFound = true;
			}
		}

		if (!fFound)
		{
			return false;
		}
	}

	return true;
}


// EOF
