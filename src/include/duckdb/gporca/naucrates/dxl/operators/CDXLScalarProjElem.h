//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarProjElem.h
//
//	@doc:
//		Class for representing DXL projection lists.
//---------------------------------------------------------------------------



#ifndef GPDXL_CDXLScalarProjElem_H
#define GPDXL_CDXLScalarProjElem_H

#include "gpos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/CMDName.h"

namespace gpdxl
{
using namespace gpmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarProjElem
//
//	@doc:
//		Container for projection list elements, storing the expression and the alias
//
//---------------------------------------------------------------------------
class CDXLScalarProjElem : public CDXLScalar
{
private:
	// id of column defined by this project element:
	// for computed columns this is a new id, for colrefs: id of the original column
	GP_ULONG m_id;

	// alias
	const CMDName *m_mdname;

	// private copy ctor
	CDXLScalarProjElem(CDXLScalarProjElem &);

public:
	// ctor/dtor
	CDXLScalarProjElem(CMemoryPool *mp, GP_ULONG id, const CMDName *mdname);

	virtual ~CDXLScalarProjElem();

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// id of the proj element
	GP_ULONG Id() const;

	// alias of the proj elem
	const CMDName *GetMdNameAlias() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// check if given column is defined by operator
	virtual GP_BOOL
	IsColDefined(GP_ULONG colid) const
	{
		return (Id() == colid);
	}

	// conversion function
	static CDXLScalarProjElem *
	Cast(CDXLOperator *dxl_op)
	{
		GPOS_ASSERT(NULL != dxl_op);
		GPOS_ASSERT(EdxlopScalarProjectElem == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarProjElem *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual GP_BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		GPOS_ASSERT(!"Invalid function call on a container operator");
		return false;
	}

#ifdef GPOS_DEBUG
	// checks whether the operator has valid structure
	void AssertValid(const CDXLNode *dxlnode, GP_BOOL validate_children) const;
#endif	// GPOS_DEBUG
};
}  // namespace gpdxl

#endif	// !GPDXL_CDXLScalarProjElem_H

// EOF
