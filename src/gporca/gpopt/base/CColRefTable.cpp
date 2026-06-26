//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CColRefTable.cpp
//
//	@doc:
//		Implementation of column reference class
//---------------------------------------------------------------------------

#include "gpopt/base/CColRefTable.h"

#include "gpos/base.h"

#include "naucrates/md/CMDIdGPDB.h"

using namespace gpopt;
using namespace gpmd;

//---------------------------------------------------------------------------
//	@function:
//		CColRefTable::CColRefTable
//
//	@doc:
//		Ctor
//		takes ownership of string; verify string is properly formatted
//
//---------------------------------------------------------------------------
CColRefTable::CColRefTable(const CColumnDescriptor *pcoldesc, GP_ULONG id,
						   const CName *pname, GP_ULONG ulOpSource)
	: CColRef(pcoldesc->RetrieveType(), pcoldesc->TypeModifier(), id, pname),
	  m_iAttno(0),
	  m_ulSourceOpId(ulOpSource),
	  m_width(pcoldesc->Width())
{
	GPOS_ASSERT(NULL != pname);

	m_iAttno = pcoldesc->AttrNum();
	m_is_nullable = pcoldesc->IsNullable();
	m_is_dist_col = pcoldesc->IsDistCol();
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefTable::CColRefTable
//
//	@doc:
//		Ctor
//		takes ownership of string; verify string is properly formatted
//
//---------------------------------------------------------------------------
CColRefTable::CColRefTable(const IMDType *pmdtype, INT type_modifier, INT attno,
						   GP_BOOL is_nullable, GP_ULONG id, const CName *pname,
						   GP_ULONG ulOpSource, GP_BOOL is_dist_col, GP_ULONG ulWidth)
	: CColRef(pmdtype, type_modifier, id, pname),
	  m_iAttno(attno),
	  m_is_nullable(is_nullable),
	  m_ulSourceOpId(ulOpSource),
	  m_is_dist_col(is_dist_col),
	  m_width(ulWidth)
{
	GPOS_ASSERT(NULL != pname);
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefTable::~CColRefTable
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CColRefTable::~CColRefTable()
{
}


// EOF
