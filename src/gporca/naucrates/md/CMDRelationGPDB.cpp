//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDRelationGPDB.cpp
//
//	@doc:
//		Implementation of the class for representing metadata cache relations
//---------------------------------------------------------------------------


#include "naucrates/md/CMDRelationGPDB.h"

#include "gpos/string/CWStringDynamic.h"

#include "naucrates/dxl/CDXLUtils.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/exception.h"

using namespace gpdxl;
using namespace gpmd;

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::CMDRelationGPDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMDRelationGPDB::CMDRelationGPDB(
	CMemoryPool *mp, IMDId *mdid, CMDName *mdname, GP_BOOL fTemporary,
	Erelstoragetype rel_storage_type, Ereldistrpolicy rel_distr_policy,
	CMDColumnArray *mdcol_array, ULongPtrArray *distr_col_array,
	IMdIdArray *distr_opfamilies, ULongPtrArray *partition_cols_array,
	CharPtrArray *str_part_types_array, GP_ULONG num_of_partitions,
	GP_BOOL convert_hash_to_random, ULongPtr2dArray *keyset_array,
	CMDIndexInfoArray *md_index_info_array, IMdIdArray *mdid_triggers_array,
	IMdIdArray *mdid_check_constraint_array,
	IMDPartConstraint *mdpart_constraint, GP_BOOL has_oids,
	IMdIdArray *external_partitions)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_mdname(mdname),
	  m_is_temp_table(fTemporary),
	  m_rel_storage_type(rel_storage_type),
	  m_rel_distr_policy(rel_distr_policy),
	  m_md_col_array(mdcol_array),
	  m_dropped_cols(0),
	  m_distr_col_array(distr_col_array),
	  m_distr_opfamilies(distr_opfamilies),
	  m_convert_hash_to_random(convert_hash_to_random),
	  m_partition_cols_array(partition_cols_array),
	  m_str_part_types_array(str_part_types_array),
	  m_num_of_partitions(num_of_partitions),
	  m_keyset_array(keyset_array),
	  m_mdindex_info_array(md_index_info_array),
	  m_mdid_trigger_array(mdid_triggers_array),
	  m_mdid_check_constraint_array(mdid_check_constraint_array),
	  m_mdpart_constraint(mdpart_constraint),
	  m_has_oids(has_oids),
	  m_system_columns(0),
	  m_colpos_nondrop_colpos_map(NULL),
	  m_attrno_nondrop_col_pos_map(NULL),
	  m_nondrop_col_pos_array(NULL),
	  m_external_partitions(external_partitions)
{
	GPOS_ASSERT(mdid->IsValid());
	GPOS_ASSERT(NULL != mdcol_array);
	GPOS_ASSERT(NULL != md_index_info_array);
	GPOS_ASSERT(NULL != mdid_triggers_array);
	GPOS_ASSERT(NULL != mdid_check_constraint_array);
	GPOS_ASSERT_IMP(
		convert_hash_to_random,
		IMDRelation::EreldistrHash == rel_distr_policy &&
			"Converting hash distributed table to random only possible for hash distributed tables");
	GPOS_ASSERT(NULL == distr_opfamilies ||
				distr_opfamilies->Size() == m_distr_col_array->Size());

	m_colpos_nondrop_colpos_map = GPOS_NEW(m_mp) UlongToUlongMap(m_mp);
	m_attrno_nondrop_col_pos_map = GPOS_NEW(m_mp) IntToUlongMap(m_mp);
	m_nondrop_col_pos_array = GPOS_NEW(m_mp) ULongPtrArray(m_mp);
	m_col_width_array = GPOS_NEW(mp) CDoubleArray(mp);

	const GP_ULONG arity = mdcol_array->Size();
	GP_ULONG non_dropped_col_pos = 0;
	for (GP_ULONG ul = 0; ul < arity; ul++)
	{
		IMDColumn *mdcol = (*mdcol_array)[ul];
		GP_BOOL is_system_col = mdcol->IsSystemColumn();
		if (is_system_col)
		{
			m_system_columns++;
		}

		(void) m_attrno_nondrop_col_pos_map->Insert(
			GPOS_NEW(m_mp) INT(mdcol->AttrNum()), GPOS_NEW(m_mp) GP_ULONG(ul));

		if (mdcol->IsDropped())
		{
			m_dropped_cols++;
		}
		else
		{
			if (!is_system_col)
			{
				m_nondrop_col_pos_array->Append(GPOS_NEW(m_mp) GP_ULONG(ul));
			}
			(void) m_colpos_nondrop_colpos_map->Insert(
				GPOS_NEW(m_mp) GP_ULONG(ul),
				GPOS_NEW(m_mp) GP_ULONG(non_dropped_col_pos));
			non_dropped_col_pos++;
		}

		m_col_width_array->Append(GPOS_NEW(mp) CDouble(mdcol->Length()));
	}
	m_dxl_str = CDXLUtils::SerializeMDObj(
		m_mp, this, false /*fSerializeHeader*/, false /*indentation*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::~CMDRelationGPDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CMDRelationGPDB::~CMDRelationGPDB()
{
	GPOS_DELETE(m_mdname);
	GPOS_DELETE(m_dxl_str);
	m_mdid->Release();
	m_md_col_array->Release();
	CRefCount::SafeRelease(m_distr_col_array);
	CRefCount::SafeRelease(m_distr_opfamilies);
	CRefCount::SafeRelease(m_partition_cols_array);
	CRefCount::SafeRelease(m_str_part_types_array);
	CRefCount::SafeRelease(m_keyset_array);
	m_mdindex_info_array->Release();
	m_mdid_trigger_array->Release();
	m_mdid_check_constraint_array->Release();
	m_col_width_array->Release();
	CRefCount::SafeRelease(m_mdpart_constraint);
	CRefCount::SafeRelease(m_colpos_nondrop_colpos_map);
	CRefCount::SafeRelease(m_attrno_nondrop_col_pos_map);
	CRefCount::SafeRelease(m_nondrop_col_pos_array);
	CRefCount::SafeRelease(m_external_partitions);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::MDId
//
//	@doc:
//		Returns the metadata id of this relation
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationGPDB::MDId() const
{
	return m_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::Mdname
//
//	@doc:
//		Returns the name of this relation
//
//---------------------------------------------------------------------------
CMDName
CMDRelationGPDB::Mdname() const
{
	return *m_mdname;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::IsTemporary
//
//	@doc:
//		Is the relation temporary
//
//---------------------------------------------------------------------------
GP_BOOL
CMDRelationGPDB::IsTemporary() const
{
	return m_is_temp_table;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::RetrieveRelStorageType
//
//	@doc:
//		Returns the storage type for this relation
//
//---------------------------------------------------------------------------
IMDRelation::Erelstoragetype
CMDRelationGPDB::RetrieveRelStorageType() const
{
	return m_rel_storage_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::GetRelDistribution
//
//	@doc:
//		Returns the distribution policy for this relation
//
//---------------------------------------------------------------------------
IMDRelation::Ereldistrpolicy
CMDRelationGPDB::GetRelDistribution() const
{
	return m_rel_distr_policy;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::ColumnCount
//
//	@doc:
//		Returns the number of columns of this relation
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::ColumnCount() const
{
	GPOS_ASSERT(NULL != m_md_col_array);

	return m_md_col_array->Size();
}

// Return the width of a column with regards to the position
DOUBLE
CMDRelationGPDB::ColWidth(GP_ULONG pos) const
{
	return (*m_col_width_array)[pos]->Get();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::HasDroppedColumns
//
//	@doc:
//		Does relation have dropped columns
//
//---------------------------------------------------------------------------
GP_BOOL
CMDRelationGPDB::HasDroppedColumns() const
{
	return 0 < m_dropped_cols;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::NonDroppedColsCount
//
//	@doc:
//		Number of non-dropped columns
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::NonDroppedColsCount() const
{
	return ColumnCount() - m_dropped_cols;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::NonDroppedColAt
//
//	@doc:
//		Return the absolute position of the given attribute position excluding
//		dropped columns
//
//		If pos is a dropped column then return gpos::ulong_max.
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::NonDroppedColAt(GP_ULONG pos) const
{
	GPOS_ASSERT(pos <= ColumnCount());

	if (!HasDroppedColumns())
	{
		return pos;
	}

	GP_ULONG *colid = m_colpos_nondrop_colpos_map->Find(&pos);

	return colid ? *colid : gpos::ulong_max;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::GetPosFromAttno
//
//	@doc:
//		Return the position of a column in the metadata object given the
//      attribute number in the system catalog
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::GetPosFromAttno(INT attno) const
{
	GP_ULONG *att_pos = m_attrno_nondrop_col_pos_map->Find(&attno);
	GPOS_ASSERT(NULL != att_pos);

	return *att_pos;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::NonDroppedColsArray
//
//	@doc:
//		Returns the original positions of all the non-dropped columns
//
//---------------------------------------------------------------------------
ULongPtrArray *
CMDRelationGPDB::NonDroppedColsArray() const
{
	return m_nondrop_col_pos_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::SystemColumnsCount
//
//	@doc:
//		Returns the number of system columns of this relation
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::SystemColumnsCount() const
{
	return m_system_columns;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::KeySetCount
//
//	@doc:
//		Returns the number of key sets
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::KeySetCount() const
{
	return (m_keyset_array == NULL) ? 0 : m_keyset_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::KeySetAt
//
//	@doc:
//		Returns the key set at the specified position
//
//---------------------------------------------------------------------------
const ULongPtrArray *
CMDRelationGPDB::KeySetAt(GP_ULONG pos) const
{
	GPOS_ASSERT(NULL != m_keyset_array);

	return (*m_keyset_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::DistrColumnCount
//
//	@doc:
//		Returns the number of columns in the distribution column list of this relation
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::DistrColumnCount() const
{
	return (m_distr_col_array == NULL) ? 0 : m_distr_col_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::HasOids
//
//	@doc:
//		Does this table have oids
//
//---------------------------------------------------------------------------
GP_BOOL
CMDRelationGPDB::HasOids() const
{
	return m_has_oids;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::IsPartitioned
//
//	@doc:
//		Is the table partitioned
//
//---------------------------------------------------------------------------
GP_BOOL
CMDRelationGPDB::IsPartitioned() const
{
	return (0 < PartColumnCount());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::PartitionCount
//
//	@doc:
//		number of partitions
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::PartitionCount() const
{
	return m_num_of_partitions;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::PartColumnCount
//
//	@doc:
//		Returns the number of partition keys
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::PartColumnCount() const
{
	return (m_partition_cols_array == NULL) ? 0
											: m_partition_cols_array->Size();
}

// Retrieve list of partition types
CharPtrArray *
CMDRelationGPDB::GetPartitionTypes() const
{
	return m_str_part_types_array;
}

// Returns the partition type of the given level
CHAR
CMDRelationGPDB::PartTypeAtLevel(GP_ULONG ulLevel) const
{
	return *(*m_str_part_types_array)[ulLevel];
}


//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::PartColAt
//
//	@doc:
//		Returns the partition column at the specified position in the
//		partition key list
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationGPDB::PartColAt(GP_ULONG pos) const
{
	GP_ULONG partition_key_pos = (*(*m_partition_cols_array)[pos]);
	return GetMdCol(partition_key_pos);
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::IndexCount
//
//	@doc:
//		Returns the number of indices of this relation
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::IndexCount() const
{
	return m_mdindex_info_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::TriggerCount
//
//	@doc:
//		Returns the number of triggers of this relation
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::TriggerCount() const
{
	return m_mdid_trigger_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::GetMdCol
//
//	@doc:
//		Returns the column at the specified position
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationGPDB::GetMdCol(GP_ULONG pos) const
{
	GPOS_ASSERT(pos < m_md_col_array->Size());

	return (*m_md_col_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::GetDistrColAt
//
//	@doc:
//		Returns the distribution column at the specified position in the distribution column list
//
//---------------------------------------------------------------------------
const IMDColumn *
CMDRelationGPDB::GetDistrColAt(GP_ULONG pos) const
{
	GPOS_ASSERT(pos < m_distr_col_array->Size());

	GP_ULONG distr_key_pos = (*(*m_distr_col_array)[pos]);
	return GetMdCol(distr_key_pos);
}

IMDId *
CMDRelationGPDB::GetDistrOpfamilyAt(GP_ULONG pos) const
{
	if (m_distr_opfamilies == NULL)
	{
		GPOS_RAISE(CException::ExmaInvalid, CException::ExmiInvalid,
				   GPOS_WSZ_LIT("GetDistrOpfamilyAt() returning NULL."));
	}

	GPOS_ASSERT(pos < m_distr_opfamilies->Size());
	return (*m_distr_opfamilies)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::ConvertHashToRandom
//
//	@doc:
//		Return true if a hash distributed table needs to be considered as random during planning
//---------------------------------------------------------------------------
GP_BOOL
CMDRelationGPDB::ConvertHashToRandom() const
{
	return m_convert_hash_to_random;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::IndexMDidAt
//
//	@doc:
//		Returns the id of the index at the specified position of the index array
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationGPDB::IndexMDidAt(GP_ULONG pos) const
{
	return (*m_mdindex_info_array)[pos]->MDId();
}

// check if index is partial given its mdid
GP_BOOL
CMDRelationGPDB::IsPartialIndex(IMDId *mdid) const
{
	const GP_ULONG indexes = IndexCount();
	bool foundPartial = false;

	// if the index is a bitmap index, there can exist both a partial and
	// non-partial entry for a given MDid. In this case, we prefer the non-partial entry
	for (GP_ULONG ul = 0; ul < indexes; ++ul)
	{
		if (CMDIdGPDB::MDIdCompare(IndexMDidAt(ul), mdid))
		{
			if (!(*m_mdindex_info_array)[ul]->IsPartial())
			{
				return false;
			}
			else
			{
				foundPartial = true;
			}
		}
	}

	if (foundPartial)
	{
		return true;
	}

	// Not found
	GPOS_RAISE(ExmaMD, ExmiMDCacheEntryNotFound, mdid->GetBuffer());

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::TriggerMDidAt
//
//	@doc:
//		Returns the id of the trigger at the specified position of the trigger array
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationGPDB::TriggerMDidAt(GP_ULONG pos) const
{
	return (*m_mdid_trigger_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::CheckConstraintCount
//
//	@doc:
//		Returns the number of check constraints on this relation
//
//---------------------------------------------------------------------------
GP_ULONG
CMDRelationGPDB::CheckConstraintCount() const
{
	return m_mdid_check_constraint_array->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::CheckConstraintMDidAt
//
//	@doc:
//		Returns the id of the check constraint at the specified position of
//		the check constraint array
//
//---------------------------------------------------------------------------
IMDId *
CMDRelationGPDB::CheckConstraintMDidAt(GP_ULONG pos) const
{
	return (*m_mdid_check_constraint_array)[pos];
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::MDPartConstraint
//
//	@doc:
//		Return the part constraint
//
//---------------------------------------------------------------------------
IMDPartConstraint *
CMDRelationGPDB::MDPartConstraint() const
{
	return m_mdpart_constraint;
}

// external partitions (for partitioned tables)
IMdIdArray *
CMDRelationGPDB::GetExternalPartitions() const
{
	return m_external_partitions;
}

//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::Serialize
//
//	@doc:
//		Serialize relation metadata in DXL format
//
//---------------------------------------------------------------------------
void
CMDRelationGPDB::Serialize(CXMLSerializer *xml_serializer) const
{
	GPOS_CHECK_ABORT;

	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenRelation));

	m_mdid->Serialize(xml_serializer,
					  CDXLTokens::GetDXLTokenStr(EdxltokenMdid));
	xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenName),
								 m_mdname->GetMDName());
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelTemporary), m_is_temp_table);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelHasOids), m_has_oids);
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelStorageType),
		IMDRelation::GetStorageTypeStr(m_rel_storage_type));
	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrPolicy),
		GetDistrPolicyStr(m_rel_distr_policy));

	if (EreldistrHash == m_rel_distr_policy)
	{
		GPOS_ASSERT(NULL != m_distr_col_array);

		// serialize distribution columns
		CWStringDynamic *distr_col_str_array =
			ColumnsToStr(m_mp, m_distr_col_array);
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenDistrColumns),
			distr_col_str_array);
		GPOS_DELETE(distr_col_str_array);
	}

	// serialize key sets
	if (m_keyset_array != NULL && m_keyset_array->Size() > 0)
	{
		CWStringDynamic *keyset_str_array =
			CDXLUtils::Serialize(m_mp, m_keyset_array);
		xml_serializer->AddAttribute(CDXLTokens::GetDXLTokenStr(EdxltokenKeys),
									 keyset_str_array);
		GPOS_DELETE(keyset_str_array);
	}

	if (IsPartitioned())
	{
		// serialize partition keys
		CWStringDynamic *part_keys_str_array =
			CDXLUtils::Serialize(m_mp, m_partition_cols_array);
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenPartKeys), part_keys_str_array);
		GPOS_DELETE(part_keys_str_array);
	}

	if (m_str_part_types_array)
	{
		// serialize partition types
		CWStringDynamic *part_types_str_array =
			CDXLUtils::SerializeToCommaSeparatedString(m_mp,
													   m_str_part_types_array);
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenPartTypes),
			part_types_str_array);
		GPOS_DELETE(part_types_str_array);
	}

	if (m_convert_hash_to_random)
	{
		xml_serializer->AddAttribute(
			CDXLTokens::GetDXLTokenStr(EdxltokenConvertHashToRandom),
			m_convert_hash_to_random);
	}

	xml_serializer->AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenNumLeafPartitions),
		m_num_of_partitions);

	// serialize columns
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenColumns));
	for (GP_ULONG ul = 0; ul < m_md_col_array->Size(); ul++)
	{
		CMDColumn *mdcol = (*m_md_col_array)[ul];
		mdcol->Serialize(xml_serializer);

		GPOS_CHECK_ABORT;
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenColumns));

	// serialize index infos
	xml_serializer->OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexInfoList));
	const GP_ULONG indexes = m_mdindex_info_array->Size();
	for (GP_ULONG ul = 0; ul < indexes; ul++)
	{
		CMDIndexInfo *index_info = (*m_mdindex_info_array)[ul];
		index_info->Serialize(xml_serializer);

		GPOS_CHECK_ABORT;
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenIndexInfoList));


	// serialize trigger information
	SerializeMDIdList(xml_serializer, m_mdid_trigger_array,
					  CDXLTokens::GetDXLTokenStr(EdxltokenTriggers),
					  CDXLTokens::GetDXLTokenStr(EdxltokenTrigger));

	// serialize check constraint information
	SerializeMDIdList(xml_serializer, m_mdid_check_constraint_array,
					  CDXLTokens::GetDXLTokenStr(EdxltokenCheckConstraints),
					  CDXLTokens::GetDXLTokenStr(EdxltokenCheckConstraint));

	// serialize operator class information, if present
	if (EreldistrHash == m_rel_distr_policy && NULL != m_distr_opfamilies)
	{
		SerializeMDIdList(
			xml_serializer, m_distr_opfamilies,
			CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpfamilies),
			CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrOpfamily));
	}

	// serialize part constraint
	if (NULL != m_mdpart_constraint)
	{
		m_mdpart_constraint->Serialize(xml_serializer);
	}

	if (HasExternalPartitions())
	{
		SerializeMDIdList(
			xml_serializer, m_external_partitions,
			CDXLTokens::GetDXLTokenStr(EdxltokenRelExternalPartitions),
			CDXLTokens::GetDXLTokenStr(EdxltokenRelExternalPartition));
	}

	xml_serializer->CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenRelation));

	GPOS_CHECK_ABORT;
}

#ifdef GPOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CMDRelationGPDB::DebugPrint
//
//	@doc:
//		Prints a metadata cache relation to the provided output
//
//---------------------------------------------------------------------------
void
CMDRelationGPDB::DebugPrint(IOstream &os) const
{
	os << "Relation id: ";
	MDId()->OsPrint(os);
	os << std::endl;

	os << "Relation name: " << (Mdname()).GetMDName()->GetBuffer() << std::endl;

	os << "Storage type: "
	   << IMDRelation::GetStorageTypeStr(m_rel_storage_type)->GetBuffer()
	   << std::endl;

	os << "Distribution policy: "
	   << GetDistrPolicyStr(m_rel_distr_policy)->GetBuffer() << std::endl;

	os << "Relation columns: " << std::endl;
	const GP_ULONG num_of_columns = ColumnCount();
	for (GP_ULONG ul = 0; ul < num_of_columns; ul++)
	{
		const IMDColumn *mdcol = GetMdCol(ul);
		mdcol->DebugPrint(os);
	}
	os << std::endl;

	os << "Distributed by: ";
	const GP_ULONG distr_columns = DistrColumnCount();
	for (GP_ULONG ul = 0; ul < distr_columns; ul++)
	{
		if (0 < ul)
		{
			os << ", ";
		}

		const IMDColumn *mdcol_distr_key = GetDistrColAt(ul);
		os << (mdcol_distr_key->Mdname()).GetMDName()->GetBuffer();
	}

	os << std::endl;

	os << "Partition keys: ";
	const GP_ULONG part_columns = PartColumnCount();
	for (GP_ULONG ul = 0; ul < part_columns; ul++)
	{
		if (0 < ul)
		{
			os << ", ";
		}

		const IMDColumn *mdcol_part_key = PartColAt(ul);
		os << (mdcol_part_key->Mdname()).GetMDName()->GetBuffer();
	}

	os << std::endl;

	os << "Index Info: ";
	const GP_ULONG indexes = m_mdindex_info_array->Size();
	for (GP_ULONG ul = 0; ul < indexes; ul++)
	{
		CMDIndexInfo *mdindex_info = (*m_mdindex_info_array)[ul];
		mdindex_info->DebugPrint(os);
	}

	os << "Triggers: ";
	CDXLUtils::DebugPrintMDIdArray(os, m_mdid_trigger_array);

	os << "Check Constraint: ";
	CDXLUtils::DebugPrintMDIdArray(os, m_mdid_check_constraint_array);
}

#endif	// GPOS_DEBUG

// EOF
