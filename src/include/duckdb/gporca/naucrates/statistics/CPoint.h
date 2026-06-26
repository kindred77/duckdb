//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPoint.h
//
//	@doc:
//		Point in the datum space
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CPoint_H
#define GPNAUCRATES_CPoint_H

#include "gpos/base.h"
#include "gpos/common/CDouble.h"

#include "naucrates/base/IDatum.h"

namespace gpopt
{
class CMDAccessor;
}

namespace gpnaucrates
{
using namespace gpos;
using namespace gpmd;
using namespace gpopt;

//---------------------------------------------------------------------------
//	@class:
//		CPoint
//
//	@doc:
//		One dimensional point in the datum space
//---------------------------------------------------------------------------
class CPoint : public CRefCount, public DbgPrintMixin<CPoint>
{
private:
	// private copy ctor
	CPoint(const CPoint &);

	// private assignment operator
	CPoint &operator=(CPoint &);

	// datum corresponding to the point
	IDatum *m_datum;

public:
	// c'tor
	explicit CPoint(IDatum *);

	// get underlying datum
	IDatum *
	GetDatum() const
	{
		return m_datum;
	}

	// is this point equal to another
	GP_BOOL Equals(const CPoint *) const;

	// is this point not equal to another
	GP_BOOL IsNotEqual(const CPoint *) const;

	// less than
	GP_BOOL IsLessThan(const CPoint *) const;

	// less than or equals
	GP_BOOL IsLessThanOrEqual(const CPoint *) const;

	// greater than
	GP_BOOL IsGreaterThan(const CPoint *) const;

	// greater than or equals
	GP_BOOL IsGreaterThanOrEqual(const CPoint *) const;

	// distance between two points
	CDouble Distance(const CPoint *) const;

	// distance between two points, taking bounds into account
	CDouble Width(const CPoint *, GP_BOOL include_lower, GP_BOOL include_upper) const;

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

	// d'tor
	virtual ~CPoint()
	{
		m_datum->Release();
	}


	// minimum of two points using <=
	static CPoint *MinPoint(CPoint *point1, CPoint *point2);

	// maximum of two points using >=
	static CPoint *MaxPoint(CPoint *point1, CPoint *point2);
};	// class CPoint

// array of CPoints
typedef CDynamicPtrArray<CPoint, CleanupRelease> CPointArray;
}  // namespace gpnaucrates

#endif	// !GPNAUCRATES_CPoint_H

// EOF
