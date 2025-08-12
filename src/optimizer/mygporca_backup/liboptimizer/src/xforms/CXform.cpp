//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXform.cpp
//
//	@doc:
//		Base class for all transformations
//---------------------------------------------------------------------------

#include "gpopt/xforms/CXform.h"

#include "gpos/base.h"


using namespace gpopt;

FORCE_GENERATE_DBGSTR(CXform);

//---------------------------------------------------------------------------
//	@function:
//		CXform::CXform
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXform::CXform(CExpression *pexpr) : m_pexpr(pexpr)
{
	GPOS_ASSERT(NULL != pexpr);
	//GPOS_ASSERT(FCheckPattern(pexpr));
}


//---------------------------------------------------------------------------
//	@function:
//		CXform::~CXform
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CXform::~CXform()
{
	m_pexpr->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CXform::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CXform::OsPrint(IOstream &os) const
{
	os << "Xform: " << SzId();

//	if (GPOS_FTRACE(EopttracePrintXformPattern))
//	{
//		os << std::endl << "Pattern:" << std::endl << *m_pexpr;
//	}

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CXform::FEqualIds
//
//	@doc:
//		Equality function on xform ids
//
//---------------------------------------------------------------------------
BOOL
CXform::FEqualIds(const CHAR *szIdOne, const CHAR *szIdTwo)
{
	return 0 == clib::Strcmp(szIdOne, szIdTwo);
}

BOOL
CXform::IsApplyOnce()
{
	return false;
}
// EOF
