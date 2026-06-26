//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLPhysicalSplit.h
//
//	@doc:
//		Class for representing physical split operator
//---------------------------------------------------------------------------

#ifndef GPDXL_CDXLPhysicalSplit_H
#define GPDXL_CDXLPhysicalSplit_H

#include "gpos/base.h"
#include "gpos/common/CDynamicPtrArray.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace gpdxl
{
// fwd decl
class CDXLTableDescr;

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalSplit
//
//	@doc:
//		Class for representing physical split operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalSplit : public CDXLPhysical
{
private:
	// list of deletion column ids
	ULongPtrArray *m_deletion_colid_array;

	// list of insertion column ids
	ULongPtrArray *m_insert_colid_array;

	// action column id
	GP_ULONG m_action_colid;

	// ctid column id
	GP_ULONG m_ctid_colid;

	// segmentid column id
	GP_ULONG m_segid_colid;

	// should update preserve tuple oids
	GP_BOOL m_preserve_oids;

	// tuple oid column id
	GP_ULONG m_tuple_oid;

	// private copy ctor
	CDXLPhysicalSplit(const CDXLPhysicalSplit &);

public:
	// ctor
	CDXLPhysicalSplit(CMemoryPool *mp, ULongPtrArray *delete_colid_array,
					  ULongPtrArray *insert_colid_array, GP_ULONG action_colid,
					  GP_ULONG ctid_colid, GP_ULONG segid_colid, GP_BOOL preserve_oids,
					  GP_ULONG tuple_oid);

	// dtor
	virtual ~CDXLPhysicalSplit();

	// operator type
	Edxlopid GetDXLOperator() const;

	// operator name
	const CWStringConst *GetOpNameStr() const;

	// deletion column ids
	ULongPtrArray *
	GetDeletionColIdArray() const
	{
		return m_deletion_colid_array;
	}

	// insertion column ids
	ULongPtrArray *
	GetInsertionColIdArray() const
	{
		return m_insert_colid_array;
	}

	// action column id
	GP_ULONG
	ActionColId() const
	{
		return m_action_colid;
	}

	// ctid column id
	GP_ULONG
	GetCtIdColId() const
	{
		return m_ctid_colid;
	}

	// segmentid column id
	GP_ULONG
	GetSegmentIdColId() const
	{
		return m_segid_colid;
	}

	// does update preserve oids
	GP_BOOL
	IsOidsPreserved() const
	{
		return m_preserve_oids;
	}

	// tuple oid column id
	GP_ULONG
	GetTupleOid() const
	{
		return m_tuple_oid;
	}

#ifdef GPOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *dxlnode, GP_BOOL validate_children) const;
#endif	// GPOS_DEBUG

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalSplit *
	Cast(CDXLOperator *dxl_op)
	{
		GPOS_ASSERT(NULL != dxl_op);
		GPOS_ASSERT(EdxlopPhysicalSplit == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalSplit *>(dxl_op);
	}
};
}  // namespace gpdxl

#endif	// !GPDXL_CDXLPhysicalSplit_H

// EOF
