//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CPattern.cpp
//
//	@doc:
//		Implementation of base class of pattern operators
//---------------------------------------------------------------------------

#include "gpopt/operators/CPattern.h"

#include "gpos/base.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CPattern::Matches
//
//	@doc:
//		match against an operator
//
//---------------------------------------------------------------------------
BOOL
CPattern::Matches(COperator *pop) const
{
	return Eopid() == pop->Eopid();
}


//---------------------------------------------------------------------------
//	@function:
//		CPattern::FInputOrderSensitive
//
//	@doc:
//		By default patterns are leaves; no need to call this function ever
//
//---------------------------------------------------------------------------
BOOL
CPattern::FInputOrderSensitive() const
{
	GPOS_ASSERT(!"Unexpected call to function FInputOrderSensitive");
	return true;
}

// EOF
