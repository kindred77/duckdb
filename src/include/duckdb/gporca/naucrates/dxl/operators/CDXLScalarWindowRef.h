//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLScalarWindowRef.h
//
//	@doc:
//		Class for representing DXL scalar WindowRef
//---------------------------------------------------------------------------

#ifndef GPDXL_CDXLScalarWindowRef_H
#define GPDXL_CDXLScalarWindowRef_H

#include "gpos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace gpdxl
{
using namespace gpos;
using namespace gpmd;

// stage of the evaluation of the window function
enum EdxlWinStage
{
	EdxlwinstageImmediate = 0,
	EdxlwinstagePreliminary,
	EdxlwinstageRowKey,
	EdxlwinstageSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarWindowRef
//
//	@doc:
//		Class for representing DXL scalar WindowRef
//
//---------------------------------------------------------------------------
class CDXLScalarWindowRef : public CDXLScalar
{
private:
	// catalog id of the function
	IMDId *m_func_mdid;

	// return type
	IMDId *m_return_type_mdid;

	// denotes whether it's agg(DISTINCT ...)
	GP_BOOL m_is_distinct;

	// is argument list really '*' //
	GP_BOOL m_is_star_arg;

	// is function a simple aggregate? //
	GP_BOOL m_is_simple_agg;

	// denotes the win stage
	EdxlWinStage m_dxl_win_stage;

	// position the window specification in a parent window operator
	GP_ULONG m_win_spec_pos;

	// private copy ctor
	CDXLScalarWindowRef(const CDXLScalarWindowRef &);

public:
	// ctor
	CDXLScalarWindowRef(CMemoryPool *mp, IMDId *pmdidWinfunc,
						IMDId *mdid_return_type, GP_BOOL is_distinct,
						GP_BOOL is_star_arg, GP_BOOL is_simple_agg,
						EdxlWinStage dxl_win_stage, GP_ULONG ulWinspecPosition);

	//dtor
	virtual ~CDXLScalarWindowRef();

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the DXL operator
	const CWStringConst *GetOpNameStr() const;

	// catalog id of the function
	IMDId *
	FuncMdId() const
	{
		return m_func_mdid;
	}

	// return type of the function
	IMDId *
	ReturnTypeMdId() const
	{
		return m_return_type_mdid;
	}

	// window stage
	EdxlWinStage
	GetDxlWinStage() const
	{
		return m_dxl_win_stage;
	}

	// denotes whether it's agg(DISTINCT ...)
	GP_BOOL
	IsDistinct() const
	{
		return m_is_distinct;
	}

	GP_BOOL
	IsStarArg() const
	{
		return m_is_star_arg;
	}

	GP_BOOL
	IsSimpleAgg() const
	{
		return m_is_simple_agg;
	}

	// position the window specification in a parent window operator
	GP_ULONG
	GetWindSpecPos() const
	{
		return m_win_spec_pos;
	}

	// set window spec position
	void
	SetWinSpecPos(GP_ULONG win_spec_pos)
	{
		m_win_spec_pos = win_spec_pos;
	}

	// string representation of win stage
	const CWStringConst *GetWindStageStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLScalarWindowRef *
	Cast(CDXLOperator *dxl_op)
	{
		GPOS_ASSERT(NULL != dxl_op);
		GPOS_ASSERT(EdxlopScalarWindowRef == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarWindowRef *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual GP_BOOL HasBoolResult(CMDAccessor *md_accessor) const;

#ifdef GPOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *dxlnode, GP_BOOL validate_children) const;
#endif	// GPOS_DEBUG
};
}  // namespace gpdxl

#endif	// !GPDXL_CDXLScalarWindowRef_H

// EOF
