//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		COperator.cpp
//
//	@doc:
//		Implementation of operator base class
//---------------------------------------------------------------------------

#include "gpopt/operators/COperator.h"

#include "gpos/base.h"

#include "gpopt/operators/CExpression.h"

using namespace gpopt;

FORCE_GENERATE_DBGSTR(COperator);

// generate unique operator ids
ULONG COperator::m_aulOpIdCounter(0);

//---------------------------------------------------------------------------
//	@function:
//		COperator::COperator
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
COperator::COperator(CMemoryPool *mp)
	: m_ulOpId(m_aulOpIdCounter++), m_mp(mp), m_fPattern(false)
{
	GPOS_ASSERT(NULL != mp);
}


//---------------------------------------------------------------------------
//	@function:
//		COperator::HashValue
//
//	@doc:
//		default hash function based on operator ID
//
//---------------------------------------------------------------------------
ULONG
COperator::HashValue() const
{
	ULONG ulEopid = (ULONG) Eopid();

	return gpos::HashValue<ULONG>(&ulEopid);
}


//---------------------------------------------------------------------------
//	@function:
//		COperator::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
COperator::OsPrint(IOstream &os) const
{
	os << this->SzId();
	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		COperator::PopCopyDefault
//
//	@doc:
//		Return an addref'ed copy of the operator
//
//---------------------------------------------------------------------------
COperator *
COperator::PopCopyDefault()
{
	this->AddRef();
	return this;
}

// EOF
