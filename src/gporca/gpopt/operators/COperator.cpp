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

#include "gpopt/base/CDrvdPropRelational.h"
#include "gpopt/base/CReqdPropRelational.h"
#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CExpressionHandle.h"

using namespace gpopt;

FORCE_GENERATE_DBGSTR(COperator);

// generate unique operator ids
GP_ULONG COperator::m_aulOpIdCounter(0);

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
GP_ULONG
COperator::HashValue() const
{
	GP_ULONG ulEopid = (GP_ULONG) Eopid();

	return gpos::HashValue<GP_ULONG>(&ulEopid);
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
//		COperator::EfsDeriveFromChildren
//
//	@doc:
//		Derive stability function property from child expressions
//
//---------------------------------------------------------------------------
IMDFunction::EFuncStbl
COperator::EfsDeriveFromChildren(CExpressionHandle &exprhdl,
								 IMDFunction::EFuncStbl efsDefault)
{
	IMDFunction::EFuncStbl efs = efsDefault;

	const GP_ULONG arity = exprhdl.Arity();
	for (GP_ULONG ul = 0; ul < arity; ul++)
	{
		IMDFunction::EFuncStbl efsChild = exprhdl.PfpChild(ul)->Efs();
		if (efsChild > efs)
		{
			efs = efsChild;
		}
	}

	return efs;
}

//---------------------------------------------------------------------------
//	@function:
//		COperator::PfpDeriveFromChildren
//
//	@doc:
//		Derive function properties from child expressions
//
//---------------------------------------------------------------------------
CFunctionProp *
COperator::PfpDeriveFromChildren(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 IMDFunction::EFuncStbl efsDefault,
								 GP_BOOL fHasVolatileFunctionScan, GP_BOOL fScan)
{
	IMDFunction::EFuncStbl efs = EfsDeriveFromChildren(exprhdl, efsDefault);

	return GPOS_NEW(mp) CFunctionProp(
		efs,
		fHasVolatileFunctionScan || exprhdl.FChildrenHaveVolatileFuncScan(),
		fScan);
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
