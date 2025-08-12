//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPattern.h
//
//	@doc:
//		Base class for all pattern operators
//---------------------------------------------------------------------------
#ifndef DUCKDB_CPATTERN_H
#define DUCKDB_CPATTERN_H

#include "gpos/base.h"

#include "gpopt/operators/COperator.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CPattern
//
//	@doc:
//		base class for all pattern operators
//
//---------------------------------------------------------------------------
class CPattern : public COperator
{
private:
	// private copy ctor
	CPattern(const CPattern &);

public:
	// ctor
	explicit CPattern(CMemoryPool *mp) : COperator(mp)
	{
	}

	// dtor
	virtual ~CPattern()
	{
	}

	// type of operator
	virtual BOOL
	FPattern() const
	{
		GPOS_ASSERT(!FPhysical() && !FScalar() && !FLogical());
		return true;
	}

	// match function
	BOOL Matches(COperator *) const;

	// sensitivity to order of inputs
	BOOL FInputOrderSensitive() const;

	// check if operator is a pattern leaf
	virtual BOOL FLeaf() const = 0;

	// conversion function
	static CPattern *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(NULL != pop);
		GPOS_ASSERT(pop->FPattern());

		return reinterpret_cast<CPattern *>(pop);
	}

	// helper to check multi-node pattern
	static BOOL
	FMultiNode(COperator *pop)
	{
		return COperator::EopPatternMultiLeaf == pop->Eopid() ||
			   COperator::EopPatternMultiTree == pop->Eopid();
	}

};	// class CPattern

}  // namespace gpopt


#endif	// !DUCKDB_CPATTERN_H

// EOF
