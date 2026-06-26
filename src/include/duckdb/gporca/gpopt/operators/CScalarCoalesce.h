//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarCoalesce.h
//
//	@doc:
//		Scalar coalesce operator
//---------------------------------------------------------------------------
#ifndef GPOPT_CScalarCoalesce_H
#define GPOPT_CScalarCoalesce_H

#include "gpos/base.h"

#include "gpopt/base/CDrvdProp.h"
#include "gpopt/operators/CScalar.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CScalarCoalesce
//
//	@doc:
//		Scalar coalesce operator
//
//---------------------------------------------------------------------------
class CScalarCoalesce : public CScalar
{
private:
	// return type
	IMDId *m_mdid_type;

	// is operator return type GP_BOOL?
	GP_BOOL m_fBoolReturnType;

	// private copy ctor
	CScalarCoalesce(const CScalarCoalesce &);

public:
	// ctor
	CScalarCoalesce(CMemoryPool *mp, IMDId *mdid_type);

	// dtor
	virtual ~CScalarCoalesce();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarCoalesce;
	}

	// operator name
	virtual const CHAR *
	SzId() const
	{
		return "CScalarCoalesce";
	}

	// return type
	virtual IMDId *
	MdidType() const
	{
		return m_mdid_type;
	}

	// operator specific hash function
	virtual GP_ULONG HashValue() const;

	// match function
	virtual GP_BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	virtual GP_BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *
	PopCopyWithRemappedColumns(CMemoryPool *,		//mp,
							   UlongToColRefMap *,	//colref_mapping,
							   GP_BOOL					//must_exist
	)
	{
		return PopCopyDefault();
	}

	// boolean expression evaluation
	virtual EBoolEvalResult
	Eber(ULongPtrArray *pdrgpulChildren) const
	{
		// Coalesce returns the first not-null child,
		// if all children are Null, then Coalesce must return Null
		return EberNullOnAllNullChildren(pdrgpulChildren);
	}

	// conversion function
	static CScalarCoalesce *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(NULL != pop);
		GPOS_ASSERT(EopScalarCoalesce == pop->Eopid());

		return dynamic_cast<CScalarCoalesce *>(pop);
	}

};	// class CScalarCoalesce

}  // namespace gpopt

#endif	// !GPOPT_CScalarCoalesce_H

// EOF
