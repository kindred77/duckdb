//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDRelationGPDB.h
//
//	@doc:
//		Class representing MD relations
//---------------------------------------------------------------------------



#ifndef GPMD_CMDRelationGPDB_H
#define GPMD_CMDRelationGPDB_H

#include "gpos/base.h"
#include "gpos/string/CWStringDynamic.h"

#include "naucrates/md/CMDColumn.h"
#include "naucrates/md/CMDName.h"
#include "naucrates/md/IMDColumn.h"
#include "naucrates/md/IMDRelation.h"

namespace gpdxl
{
class CXMLSerializer;
}

namespace gpmd
{
using namespace gpos;
using namespace gpdxl;


//---------------------------------------------------------------------------
//	@class:
//		CMDRelationGPDB
//
//	@doc:
//		Class representing MD relations
//
//---------------------------------------------------------------------------
class CMDRelationGPDB : public IMDRelation
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// DXL for object
	const CWStringDynamic *m_dxl_str;

	// relation mdid
	IMDId *m_mdid;

	// table name
	CMDName *m_mdname;

	// is this a temporary relation
	GP_BOOL m_is_temp_table;

	// storage type
	Erelstoragetype m_rel_storage_type;

	// distribution policy
	Ereldistrpolicy m_rel_distr_policy;

	// columns
	CMDColumnArray *m_md_col_array;

	// number of dropped columns
	GP_ULONG m_dropped_cols;

	// indices of distribution columns
	ULongPtrArray *m_distr_col_array;

	IMdIdArray *m_distr_opfamilies;

	// do we need to consider a hash distributed table as random distributed
	GP_BOOL m_convert_hash_to_random;

	// indices of partition columns
	ULongPtrArray *m_partition_cols_array;

	// partition types
	CharPtrArray *m_str_part_types_array;

	// number of partition
	GP_ULONG m_num_of_partitions;

	// array of key sets
	ULongPtr2dArray *m_keyset_array;

	// array of index info
	CMDIndexInfoArray *m_mdindex_info_array;

	// array of trigger ids
	IMdIdArray *m_mdid_trigger_array;

	// array of check constraint mdids
	IMdIdArray *m_mdid_check_constraint_array;

	// partition constraint
	IMDPartConstraint *m_mdpart_constraint;

	// does this table have oids
	GP_BOOL m_has_oids;

	// number of system columns
	GP_ULONG m_system_columns;

	// mapping of column position to positions excluding dropped columns
	UlongToUlongMap *m_colpos_nondrop_colpos_map;

	// mapping of attribute number in the system catalog to the positions of
	// the non dropped column in the metadata object
	IntToUlongMap *m_attrno_nondrop_col_pos_map;

	// the original positions of all the non-dropped columns
	ULongPtrArray *m_nondrop_col_pos_array;

	// array of column widths including dropped columns
	CDoubleArray *m_col_width_array;

	// oids of any external partitions (for partitioned tables only)
	IMdIdArray *m_external_partitions;

	// private copy ctor
	CMDRelationGPDB(const CMDRelationGPDB &);

public:
	// ctor
	CMDRelationGPDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
					GP_BOOL is_temp_table, Erelstoragetype rel_storage_type,
					Ereldistrpolicy rel_distr_policy,
					CMDColumnArray *mdcol_array, ULongPtrArray *distr_col_array,
					IMdIdArray *distr_opfamilies,
					ULongPtrArray *partition_cols_array,
					CharPtrArray *str_part_types_array, GP_ULONG num_of_partitions,
					GP_BOOL convert_hash_to_random, ULongPtr2dArray *keyset_array,
					CMDIndexInfoArray *md_index_info_array,
					IMdIdArray *mdid_triggers_array,
					IMdIdArray *mdid_check_constraint_array,
					IMDPartConstraint *mdpart_constraint, GP_BOOL has_oids,
					IMdIdArray *external_partitions);

	// dtor
	virtual ~CMDRelationGPDB();

	// accessors
	virtual const CWStringDynamic *
	GetStrRepr() const
	{
		return m_dxl_str;
	}

	// the metadata id
	virtual IMDId *MDId() const;

	// relation name
	virtual CMDName Mdname() const;

	// is this a temp relation
	virtual GP_BOOL IsTemporary() const;

	// storage type (heap, appendonly, ...)
	virtual Erelstoragetype RetrieveRelStorageType() const;

	// distribution policy (none, hash, random)
	virtual Ereldistrpolicy GetRelDistribution() const;

	// number of columns
	virtual GP_ULONG ColumnCount() const;

	// width of a column with regards to the position
	virtual DOUBLE ColWidth(GP_ULONG pos) const;

	// does relation have dropped columns
	virtual GP_BOOL HasDroppedColumns() const;

	// number of non-dropped columns
	virtual GP_ULONG NonDroppedColsCount() const;

	// return the absolute position of the given attribute position excluding dropped columns
	virtual GP_ULONG NonDroppedColAt(GP_ULONG pos) const;

	// return the position of a column in the metadata object given the attribute number in the system catalog
	virtual GP_ULONG GetPosFromAttno(INT attno) const;

	// return the original positions of all the non-dropped columns
	virtual ULongPtrArray *NonDroppedColsArray() const;

	// number of system columns
	virtual GP_ULONG SystemColumnsCount() const;

	// retrieve the column at the given position
	virtual const IMDColumn *GetMdCol(GP_ULONG pos) const;

	// number of key sets
	virtual GP_ULONG KeySetCount() const;

	// key set at given position
	virtual const ULongPtrArray *KeySetAt(GP_ULONG pos) const;

	// number of distribution columns
	virtual GP_ULONG DistrColumnCount() const;

	// retrieve the column at the given position in the distribution columns list for the relation
	virtual const IMDColumn *GetDistrColAt(GP_ULONG pos) const;

	virtual IMDId *GetDistrOpfamilyAt(GP_ULONG pos) const;

	// return true if a hash distributed table needs to be considered as random
	virtual GP_BOOL ConvertHashToRandom() const;

	// does this table have oids
	virtual GP_BOOL HasOids() const;

	// is this a partitioned table
	virtual GP_BOOL IsPartitioned() const;

	// number of partition keys
	virtual GP_ULONG PartColumnCount() const;

	// number of partitions
	virtual GP_ULONG PartitionCount() const;

	// retrieve the partition key column at the given position
	virtual const IMDColumn *PartColAt(GP_ULONG pos) const;

	// retrieve list of partition types
	virtual CharPtrArray *GetPartitionTypes() const;

	// retrieve the partition type of the given level
	virtual CHAR PartTypeAtLevel(GP_ULONG ulLevel) const;

	// number of indices
	virtual GP_ULONG IndexCount() const;

	// number of triggers
	virtual GP_ULONG TriggerCount() const;

	// retrieve the id of the metadata cache index at the given position
	virtual IMDId *IndexMDidAt(GP_ULONG pos) const;

	// check if index is partial given its mdid
	virtual GP_BOOL IsPartialIndex(IMDId *mdid) const;

	// retrieve the id of the metadata cache trigger at the given position
	virtual IMDId *TriggerMDidAt(GP_ULONG pos) const;

	// serialize metadata relation in DXL format given a serializer object
	virtual void Serialize(gpdxl::CXMLSerializer *) const;

	// number of check constraints
	virtual GP_ULONG CheckConstraintCount() const;

	// retrieve the id of the check constraint cache at the given position
	virtual IMDId *CheckConstraintMDidAt(GP_ULONG pos) const;

	// part constraint
	virtual IMDPartConstraint *MDPartConstraint() const;

	// external partitions (for partitioned tables)
	virtual IMdIdArray *GetExternalPartitions() const;

#ifdef GPOS_DEBUG
	// debug print of the metadata relation
	virtual void DebugPrint(IOstream &os) const;
#endif
};
}  // namespace gpmd



#endif	// !GPMD_CMDRelationGPDB_H

// EOF
