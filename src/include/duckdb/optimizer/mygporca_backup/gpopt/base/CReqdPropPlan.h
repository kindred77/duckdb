//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC CORP.
//
//	@filename:
//		CReqdPropPlan.h
//
//	@doc:
//		Derived required relational properties
//---------------------------------------------------------------------------
#ifndef DUCKDB_CREQDPROPPLAN_H
#define DUCKDB_CREQDPROPPLAN_H

#include "gpos/base.h"

#include "gpopt/base/CReqdProp.h"

namespace gpopt
{
using namespace gpos;

// forward declaration
class CDrvdPropRelational;
class CDrvdPropPlan;
class CExpressionHandle;

//---------------------------------------------------------------------------
//	@class:
//		CReqdPropPlan
//
//	@doc:
//		Required plan properties container.
//
//---------------------------------------------------------------------------
class CReqdPropPlan : public CReqdProp
{
private:

	// private copy ctor
	CReqdPropPlan(const CReqdPropPlan &);

public:
	// default ctor
	CReqdPropPlan()
		: m_pcrs(NULL)
	{
	}

	// dtor
	virtual ~CReqdPropPlan();

	// required properties computation function
	virtual void Compute(CMemoryPool *mp, CExpressionHandle &exprhdl,
						 CReqdProp *prpInput, ULONG child_index,
						 CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

	// required columns computation function
	void ComputeReqdCols(CMemoryPool *mp, CExpressionHandle &exprhdl,
						 CReqdProp *prpInput, ULONG child_index,
						 CDrvdPropArray *pdrgpdpCtxt);

	// required ctes computation function
	void ComputeReqdCTEs(CMemoryPool *mp, CExpressionHandle &exprhdl,
						 CReqdProp *prpInput, ULONG child_index,
						 CDrvdPropArray *pdrgpdpCtxt);

	// equality function
	BOOL Equals(const CReqdPropPlan *prpp) const;

	// hash function
	ULONG HashValue() const;

	// check if plan properties are satisfied by the given derived properties
	BOOL FSatisfied(const CDrvdPropRelational *pdprel,
					const CDrvdPropPlan *pdpplan) const;

	// check if expression attached to handle provides required columns by all plan properties
	BOOL FProvidesReqdCols(CMemoryPool *mp, CExpressionHandle &exprhdl,
						   ULONG ulOptReq) const;

	// shorthand for conversion
	static CReqdPropPlan *
	Prpp(CReqdProp *prp)
	{
		GPOS_ASSERT(NULL != prp);

		return dynamic_cast<CReqdPropPlan *>(prp);
	}

	//generate empty required properties
	static CReqdPropPlan *PrppEmpty(CMemoryPool *mp);

	// hash function used for cost bounding
	static ULONG UlHashForCostBounding(const CReqdPropPlan *prpp);

	// equality function used for cost bounding
	static BOOL FEqualForCostBounding(const CReqdPropPlan *prppFst,
									  const CReqdPropPlan *prppSnd);

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CReqdPropPlan

}  // namespace gpopt


#endif	// !DUCKDB_CREQDPROPPLAN_H

// EOF
