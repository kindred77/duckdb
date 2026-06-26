//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarWindowFunc.h
//
//	@doc:
//		Class for scalar window function
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------
#ifndef GPOPT_CScalarWindowFunc_H
#define GPOPT_CScalarWindowFunc_H

#include "gpos/base.h"

#include "gpopt/base/CDrvdProp.h"
#include "gpopt/base/COptCtxt.h"
#include "gpopt/operators/CScalarFunc.h"
#include "naucrates/md/IMDId.h"

namespace gpopt
{
using namespace gpos;
using namespace gpmd;

//---------------------------------------------------------------------------
//	@class:
//		CScalarWindowFunc
//
//	@doc:
//		Class for scalar window function
//
//---------------------------------------------------------------------------
class CScalarWindowFunc : public CScalarFunc
{
public:
	// window stage
	enum EWinStage
	{
		EwsImmediate,
		EwsPreliminary,
		EwsRowKey,

		EwsSentinel
	};

private:
	// window stage
	EWinStage m_ewinstage;

	// distinct window computation
	GP_BOOL m_is_distinct;

	/* TRUE if argument list was really '*' */
	GP_BOOL m_is_star_arg;

	/* is function a simple aggregate? */
	GP_BOOL m_is_simple_agg;

	// aggregate window function, e.g. count(*) over()
	GP_BOOL m_fAgg;

	// private copy ctor
	CScalarWindowFunc(const CScalarWindowFunc &);

public:
	// ctor
	CScalarWindowFunc(CMemoryPool *mp, IMDId *mdid_func,
					  IMDId *mdid_return_type, const CWStringConst *pstrFunc,
					  EWinStage ewinstage, GP_BOOL is_distinct, GP_BOOL is_star_arg,
					  GP_BOOL is_simple_agg);

	// dtor
	virtual ~CScalarWindowFunc()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopScalarWindowFunc;
	}

	// return a string for window function
	virtual const CHAR *
	SzId() const
	{
		return "CScalarWindowFunc";
	}

	EWinStage
	Ews() const
	{
		return m_ewinstage;
	}

	// operator specific hash function
	GP_ULONG HashValue() const;

	// match function
	GP_BOOL Matches(COperator *pop) const;

	// conversion function
	static CScalarWindowFunc *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(NULL != pop);
		GPOS_ASSERT(EopScalarWindowFunc == pop->Eopid());

		return reinterpret_cast<CScalarWindowFunc *>(pop);
	}

	// does window function definition include Distinct?
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

	// is window function defined as Aggregate?
	GP_BOOL
	FAgg() const
	{
		return m_fAgg;
	}

	// print
	virtual IOstream &OsPrint(IOstream &os) const;


};	// class CScalarWindowFunc

}  // namespace gpopt

#endif	// !GPOPT_CScalarWindowFunc_H

// EOF
