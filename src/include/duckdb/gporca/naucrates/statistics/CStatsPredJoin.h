//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CStatsPredJoin.h
//
//	@doc:
//		Join predicate used for join cardinality estimation
//---------------------------------------------------------------------------
#ifndef GPNAUCRATES_CStatsPredJoin_H
#define GPNAUCRATES_CStatsPredJoin_H

#include "gpos/base.h"
#include "gpos/common/CDynamicPtrArray.h"
#include "gpos/common/CRefCount.h"
#include "gpos/common/CDouble.h"

#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CStatsPred.h"

namespace gpnaucrates
{
using namespace gpos;
using namespace gpmd;
using namespace gpopt;

//---------------------------------------------------------------------------
//	@class:
//		CStatsPredJoin
//
//	@doc:
//		Join predicate used for join cardinality estimation
//---------------------------------------------------------------------------
class CStatsPredJoin : public CRefCount
{
private:
	// private copy ctor
	CStatsPredJoin(const CStatsPredJoin &);

	// private assignment operator
	CStatsPredJoin &operator=(CStatsPredJoin &);

	// column id
	GP_ULONG m_colidOuter;

	// comparison type
	CStatsPred::EStatsCmpType m_stats_cmp_type;

	// column id
	GP_ULONG m_colidInner;

public:
	// c'tor
	CStatsPredJoin(GP_ULONG colid1, CStatsPred::EStatsCmpType stats_cmp_type,
				   GP_ULONG colid2)
		: m_colidOuter(colid1),
		  m_stats_cmp_type(stats_cmp_type),
		  m_colidInner(colid2)
	{
	}

	// accessors
	GP_BOOL
	HasValidColIdOuter() const
	{
		return gpos::ulong_max != m_colidOuter;
	}

	GP_ULONG
	ColIdOuter() const
	{
		return m_colidOuter;
	}

	// comparison type
	CStatsPred::EStatsCmpType
	GetCmpType() const
	{
		return m_stats_cmp_type;
	}

	GP_BOOL
	HasValidColIdInner() const
	{
		return gpos::ulong_max != m_colidInner;
	}

	GP_ULONG
	ColIdInner() const
	{
		return m_colidInner;
	}

	// d'tor
	virtual ~CStatsPredJoin()
	{
	}

};	// class CStatsPredJoin

// array of filters
typedef CDynamicPtrArray<CStatsPredJoin, CleanupRelease> CStatsPredJoinArray;
}  // namespace gpnaucrates

#endif	// !GPNAUCRATES_CStatsPredJoin_H

// EOF
