//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CScalarProjectList.h
//
//	@doc:
//		Projection list
//---------------------------------------------------------------------------
#ifndef GPOPT_CScalarProjectList_H
#define GPOPT_CScalarProjectList_H

#include "gpos/base.h"

#include "gpopt/base/CDrvdProp.h"
#include "gpopt/operators/CScalar.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarProjectList
//
//	@doc:
//		Projection list operator
//
//---------------------------------------------------------------------------
class CScalarProjectList : public CScalar
{
private:
	// private copy ctor
	CScalarProjectList(const CScalarProjectList &);

public:
	// ctor
	explicit CScalarProjectList(CMemoryPool *mp);

	// dtor
	virtual ~CScalarProjectList()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarProjectList;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarProjectList";
	}

	// match function
	GP_BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	GP_BOOL FInputOrderSensitive() const;

	// return a copy of the operator with remapped columns
	virtual COperator *
	PopCopyWithRemappedColumns(CMemoryPool *,		//mp,
							   UlongToColRefMap *,	//colref_mapping,
							   GP_BOOL					//must_exist
	)
	{
		return PopCopyDefault();
	}

	// conversion function
	static CScalarProjectList *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(NULL != pop);
		GPOS_ASSERT(EopScalarProjectList == pop->Eopid());

		return reinterpret_cast<CScalarProjectList *>(pop);
	}

	virtual IMDId *
	MdidType() const
	{
		GPOS_ASSERT(!"Invalid function call: CScalarProjectList::MdidType()");
		return NULL;
	}

	// return number of distinct aggs in project list attached to given handle
	static GP_ULONG UlDistinctAggs(CExpressionHandle &exprhdl);

	// return number of ordered aggs in project list attached to given handle
	static GP_ULONG UlOrderedAggs(CExpressionHandle &exprhdl);

	// check if a project list has multiple distinct aggregates
	static GP_BOOL FHasMultipleDistinctAggs(CExpressionHandle &exprhdl);

	// check if a project list has a scalar func
	static GP_BOOL FHasScalarFunc(CExpressionHandle &exprhdl);

};	// class CScalarProjectList

}  // namespace gpopt


#endif	// !GPOPT_CScalarProjectList_H

// EOF
