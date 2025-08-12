//
// Created by admin on 2025/8/11.
//

#ifndef DUCKDB_COPERATOR_H
#define DUCKDB_COPERATOR_H

#include "gpos/base.h"
#include "gpos/common/CHashMap.h"
#include "gpos/common/CRefCount.h"
#include "gpos/common/DbgPrintMixin.h"

namespace gpopt
{
using namespace gpos;

// forward declarations
class COperator;

// dynamic array for operators
typedef CDynamicPtrArray<COperator, CleanupRelease> COperatorArray;

class COperator : public CRefCount, public DbgPrintMixin<COperator>
{
private:
	// private copy ctor
	COperator(COperator &);

protected:
	// operator id that is unique over all instances of all operator types
	// for the current query
	ULONG m_ulOpId;

	// memory pool for internal allocations
	CMemoryPool *m_mp;

	// is pattern of xform
	BOOL m_fPattern;

	// return an addref'ed copy of the operator
	virtual COperator *PopCopyDefault();

	// generate unique operator ids
	static ULONG m_aulOpIdCounter;

public:
	// identification
	enum EOperatorId
	{
		EopLogicalGet,

		EopPatternTree,
		EopPatternLeaf,
		EopPatternMultiLeaf,
		EopPatternMultiTree,
		EopPatternNode,

	};
	explicit COperator(CMemoryPool *mp);

	// dtor
	virtual ~COperator()
	{
	}

	// the id of the operator
	ULONG
	UlOpId() const
	{
		return m_ulOpId;
	}

	// ident accessors
	virtual EOperatorId Eopid() const = 0;

	// return a string for operator name
	virtual const CHAR *SzId() const = 0;

	// the following functions check operator's type

	// is operator logical?
	virtual BOOL
	FLogical() const
	{
		return false;
	}

	// is operator physical?
	virtual BOOL
	FPhysical() const
	{
		return false;
	}

	// is operator scalar?
	virtual BOOL
	FScalar() const
	{
		return false;
	}

	// is operator pattern?
	virtual BOOL
	FPattern() const
	{
		return false;
	}

	// hash function
	virtual ULONG HashValue() const;

	// sensitivity to order of inputs
	virtual BOOL FInputOrderSensitive() const = 0;

	// match function;
	// abstract to enforce an implementation for each new operator
	virtual BOOL Matches(COperator *pop) const = 0;

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class COperator

}  // namespace gpopt


#endif	//