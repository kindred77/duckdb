//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		IMDRelation.h
//
//	@doc:
//		Interface for relations in the metadata cache
//---------------------------------------------------------------------------

#ifndef GPMD_IMDRelation_H
#define GPMD_IMDRelation_H

#include "gpos/base.h"

#include "naucrates/md/CMDIndexInfo.h"
#include "naucrates/md/IMDCacheObject.h"
#include "naucrates/md/IMDColumn.h"
#include "naucrates/md/IMDPartConstraint.h"
#include "naucrates/statistics/IStatistics.h"

namespace gpdxl
{
//fwd declaration
class CXMLSerializer;
}  // namespace gpdxl

namespace gpmd
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		IMDRelation
//
//	@doc:
//		Interface for relations in the metadata cache
//
//---------------------------------------------------------------------------
class IMDRelation : public IMDCacheObject
{
public:
	//-------------------------------------------------------------------
	//	@doc:
	//		Storage type of a relation
	//-------------------------------------------------------------------
	enum Erelstoragetype
	{
		ErelstorageHeap,
		ErelstorageAppendOnlyCols,
		ErelstorageAppendOnlyRows,
		ErelstorageAppendOnlyParquet,
		ErelstorageExternal,
		ErelstorageVirtual,
		ErelstorageSentinel
	};

	//-------------------------------------------------------------------
	//	@doc:
	//		Distribution policy of a relation
	//-------------------------------------------------------------------
	enum Ereldistrpolicy
	{
		EreldistrMasterOnly,
		EreldistrHash,
		EreldistrRandom,
		EreldistrReplicated,
		EreldistrSentinel
	};

	// Partition type of a partitioned relation
	enum Erelpartitiontype
	{
		ErelpartitionRange = 'r',
		ErelpartitionList = 'l'
	};

protected:
	// serialize an array of column ids into a comma-separated string
	static CWStringDynamic *ColumnsToStr(CMemoryPool *mp,
										 ULongPtrArray *colid_array);

public:
	// object type
	virtual Emdtype
	MDType() const
	{
		return EmdtRel;
	}

	// is this a temp relation
	virtual GP_BOOL IsTemporary() const = 0;

	// storage type (heap, appendonly, ...)
	virtual Erelstoragetype RetrieveRelStorageType() const = 0;

	// distribution policy (none, hash, random)
	virtual Ereldistrpolicy GetRelDistribution() const = 0;

	// number of columns
	virtual GP_ULONG ColumnCount() const = 0;

	// width of a column with regards to the position
	virtual DOUBLE ColWidth(GP_ULONG pos) const = 0;

	// does relation have dropped columns
	virtual GP_BOOL HasDroppedColumns() const = 0;

	// number of non-dropped columns
	virtual GP_ULONG NonDroppedColsCount() const = 0;

	// return the position of the given attribute position excluding dropped columns
	virtual GP_ULONG NonDroppedColAt(GP_ULONG pos) const = 0;

	// return the position of a column in the metadata object given the attribute number in the system catalog
	virtual GP_ULONG GetPosFromAttno(INT attno) const = 0;

	// return the original positions of all the non-dropped columns
	virtual ULongPtrArray *NonDroppedColsArray() const = 0;

	// number of system columns
	virtual GP_ULONG SystemColumnsCount() const = 0;

	// retrieve the column at the given position
	virtual const IMDColumn *GetMdCol(GP_ULONG pos) const = 0;

	// number of key sets
	virtual GP_ULONG KeySetCount() const = 0;

	// key set at given position
	virtual const ULongPtrArray *KeySetAt(GP_ULONG pos) const = 0;

	// number of distribution columns
	virtual GP_ULONG DistrColumnCount() const = 0;

	// retrieve the column at the given position in the distribution key for the relation
	virtual const IMDColumn *GetDistrColAt(GP_ULONG pos) const = 0;

	virtual IMDId *GetDistrOpfamilyAt(GP_ULONG pos) const = 0;

	// return true if a hash distributed table needs to be considered as random
	virtual GP_BOOL ConvertHashToRandom() const = 0;

	// does this table have oids
	virtual GP_BOOL HasOids() const = 0;

	// is this a partitioned table
	virtual GP_BOOL IsPartitioned() const = 0;

	// number of partition columns
	virtual GP_ULONG PartColumnCount() const = 0;

	// number of partitions
	virtual GP_ULONG PartitionCount() const = 0;

	// retrieve the partition column at the given position
	virtual const IMDColumn *PartColAt(GP_ULONG pos) const = 0;

	// retrieve list of partition types
	virtual CharPtrArray *GetPartitionTypes() const = 0;

	// retrieve the partition type of the given partition level
	virtual CHAR PartTypeAtLevel(GP_ULONG pos) const = 0;

	// number of indices
	virtual GP_ULONG IndexCount() const = 0;

	// number of triggers
	virtual GP_ULONG TriggerCount() const = 0;

	// retrieve the id of the metadata cache index at the given position
	virtual IMDId *IndexMDidAt(GP_ULONG pos) const = 0;

	// check if index is partial given its mdid
	virtual GP_BOOL IsPartialIndex(IMDId *mdid) const;

	// retrieve the id of the metadata cache trigger at the given position
	virtual IMDId *TriggerMDidAt(GP_ULONG pos) const = 0;

	// number of check constraints
	virtual GP_ULONG CheckConstraintCount() const = 0;

	// retrieve the id of the check constraint cache at the given position
	virtual IMDId *CheckConstraintMDidAt(GP_ULONG pos) const = 0;

	// part constraint
	virtual IMDPartConstraint *MDPartConstraint() const = 0;

	// external partitions (for partitioned tables)
	virtual IMdIdArray *
	GetExternalPartitions() const
	{
		return NULL;
	}

	// contains any external partitions (for partitioned tables only)
	GP_BOOL
	HasExternalPartitions() const
	{
		return (NULL != GetExternalPartitions() &&
				GetExternalPartitions()->Size() > 0);
	}

	// relation distribution policy as a string value
	static const CWStringConst *GetDistrPolicyStr(
		Ereldistrpolicy rel_distr_policy);

	// name of storage type
	static const CWStringConst *GetStorageTypeStr(
		IMDRelation::Erelstoragetype rel_storage_type);

	GP_BOOL
	IsAORowOrColTable() const
	{
		Erelstoragetype st = RetrieveRelStorageType();
		return st == ErelstorageAppendOnlyCols ||
			   st == ErelstorageAppendOnlyRows;
	}
};

// common structure over relation and external relation metadata for index info
typedef CDynamicPtrArray<CMDIndexInfo, CleanupRelease> CMDIndexInfoArray;

}  // namespace gpmd



#endif	// !GPMD_IMDRelation_H

// EOF
