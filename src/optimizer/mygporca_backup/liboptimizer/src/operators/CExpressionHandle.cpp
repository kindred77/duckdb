//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CExpressionHandle.cpp
//
//	@doc:
//		Handle to an expression to abstract topology;
//
//		The handle provides access to an expression and the properties
//		of its children; regardless of whether the expression is a group
//		expression or a stand-alone tree;
//---------------------------------------------------------------------------

#include "gpopt/operators/CExpressionHandle.h"

#include "gpos/base.h"

#include "gpopt/base/CCostContext.h"
#include "gpopt/base/CReqdPropPlan.h"
#include "gpopt/exception.h"
#include "gpopt/operators/COperator.h"
#include "gpopt/operators/CPattern.h"

using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::CExpressionHandle
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CExpressionHandle::CExpressionHandle(CMemoryPool *mp)
	: m_mp(mp),
	  m_pexpr(NULL),
	  m_pgexpr(NULL),
	  m_pcc(NULL),
	  m_pdpplan(NULL),
	  m_prp(NULL),
	  m_pdrgprp(NULL)
{
	GPOS_ASSERT(NULL != mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::~CExpressionHandle
//
//	@doc:
//		dtor
//
//		Since handles live on the stack this dtor will be called during
//		exceptions, hence, need to be defensive
//
//---------------------------------------------------------------------------
CExpressionHandle::~CExpressionHandle()
{
	CRefCount::SafeRelease(m_pexpr);
	CRefCount::SafeRelease(m_pgexpr);
	CRefCount::SafeRelease(m_prp);
	CRefCount::SafeRelease(m_pdpplan);
	CRefCount::SafeRelease(m_pdrgprp);
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Attach
//
//	@doc:
//		Attach to a given expression
//
//---------------------------------------------------------------------------
void
CExpressionHandle::Attach(CExpression *pexpr)
{
	GPOS_ASSERT(NULL == m_pexpr);
	GPOS_ASSERT(NULL == m_pgexpr);
	GPOS_ASSERT(NULL != pexpr);

	// increment ref count on base expression
	pexpr->AddRef();
	m_pexpr = pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Attach
//
//	@doc:
//		Attach to a given group expression
//
//---------------------------------------------------------------------------
void
CExpressionHandle::Attach(CGroupExpression *pgexpr)
{
	GPOS_ASSERT(NULL == m_pexpr);
	GPOS_ASSERT(NULL == m_pgexpr);
	GPOS_ASSERT(NULL != pgexpr);

	// increment ref count on group expression
	pgexpr->AddRef();
	m_pgexpr = pgexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Attach
//
//	@doc:
//		Attach to a given cost context
//
//---------------------------------------------------------------------------
void
CExpressionHandle::Attach(CCostContext *pcc)
{
	GPOS_ASSERT(NULL == m_pcc);
	GPOS_ASSERT(NULL != pcc);

	m_pcc = pcc;
	Attach(pcc->Pgexpr());
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::DeriveProps
//
//	@doc:
//		Recursive property derivation
//
//---------------------------------------------------------------------------
void
CExpressionHandle::DeriveProps(CDrvdPropCtxt *pdpctxt)
{

}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::FAttachedToLeafPattern
//
//	@doc:
//		Return True if handle is attached to a leaf pattern
//
//---------------------------------------------------------------------------
BOOL
CExpressionHandle::FAttachedToLeafPattern() const
{
	return 0 == Arity() && NULL != m_pexpr && NULL != m_pexpr->Pgexpr();
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::InitReqdProps
//
//	@doc:
//		Init required properties containers
//
//
//---------------------------------------------------------------------------
void
CExpressionHandle::InitReqdProps(CReqdProp *prpInput)
{
	GPOS_ASSERT(NULL != prpInput);
	GPOS_ASSERT(NULL == m_prp);
	GPOS_ASSERT(NULL == m_pdrgprp);

	// set required properties of attached expr/gexpr
	m_prp = prpInput;
	m_prp->AddRef();

	if (m_prp->FPlan())
	{
		CReqdPropPlan *prpp = CReqdPropPlan::Prpp(prpInput);
		if (NULL == prpp->Pepp())
		{
			//CPartInfo *ppartinfo = DerivePartitionInfo();
			//prpp->InitReqdPartitionPropagation(m_mp, ppartinfo);
		}
	}

	// compute required properties of children
	m_pdrgprp = GPOS_NEW(m_mp) CReqdPropArray(m_mp);

	// initialize array with input requirements,
	// the initial requirements are only place holders in the array
	// and they are replaced when computing the requirements of each child
	const ULONG arity = Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		m_prp->AddRef();
		m_pdrgprp->Append(m_prp);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::ComputeChildReqdProps
//
//	@doc:
//		Compute required properties of the n-th child
//
//
//---------------------------------------------------------------------------
void
CExpressionHandle::ComputeChildReqdProps(ULONG child_index,
										 CDrvdPropArray *pdrgpdpCtxt,
										 ULONG ulOptReq)
{
	GPOS_ASSERT(NULL != m_prp);
	GPOS_ASSERT(NULL != m_pdrgprp);
	GPOS_ASSERT(m_pdrgprp->Size() == Arity());
	GPOS_ASSERT(child_index < m_pdrgprp->Size() &&
				"uninitialized required child properties");
	GPOS_CHECK_ABORT;

	CReqdProp *prp = m_prp;
	if (FScalarChild(child_index))
	{
		// use local reqd properties to fill scalar child entry in children array
		prp->AddRef();
	}
	else
	{
		// compute required properties based on child type
		//prp = Pop()->PrpCreate(m_mp);
		//prp->Compute(m_mp, *this, m_prp, child_index, pdrgpdpCtxt, ulOptReq);
	}

	// replace required properties of given child
	m_pdrgprp->Replace(child_index, prp);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::CopyChildReqdProps
//
//	@doc:
//		Copy required properties of the n-th child
//
//
//---------------------------------------------------------------------------
void
CExpressionHandle::CopyChildReqdProps(ULONG child_index, CReqdProp *prp)
{
	GPOS_ASSERT(NULL != prp);
	GPOS_ASSERT(NULL != m_pdrgprp);
	GPOS_ASSERT(m_pdrgprp->Size() == Arity());
	GPOS_ASSERT(child_index < m_pdrgprp->Size() &&
				"uninitialized required child properties");

	m_pdrgprp->Replace(child_index, prp);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::ComputeChildReqdCols
//
//	@doc:
//		Compute required columns of the n-th child
//
//
//---------------------------------------------------------------------------
void
CExpressionHandle::ComputeChildReqdCols(ULONG child_index,
										CDrvdPropArray *pdrgpdpCtxt)
{
	GPOS_ASSERT(NULL != m_prp);
	GPOS_ASSERT(NULL != m_pdrgprp);
	GPOS_ASSERT(m_pdrgprp->Size() == Arity());
	GPOS_ASSERT(child_index < m_pdrgprp->Size() &&
				"uninitialized required child properties");

	CReqdProp *prp = m_prp;
	if (FScalarChild(child_index))
	{
		// use local reqd properties to fill scalar child entry in children array
		prp->AddRef();
	}
	else
	{
		// compute required columns
		//prp = Pop()->PrpCreate(m_mp);
		//CReqdPropPlan::Prpp(prp)->ComputeReqdCols(m_mp, *this, m_prp,
		//										  child_index, pdrgpdpCtxt);
	}

	// replace required properties of given child
	m_pdrgprp->Replace(child_index, prp);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::ComputeReqdProps
//
//	@doc:
//		Set required properties of attached expr/gexpr, and compute required
//		properties of all children
//
//---------------------------------------------------------------------------
void
CExpressionHandle::ComputeReqdProps(CReqdProp *prpInput, ULONG ulOptReq)
{
	InitReqdProps(prpInput);
	const ULONG arity = Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		ComputeChildReqdProps(ul, NULL /*pdrgpdpCtxt*/, ulOptReq);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::FScalarChild
//
//	@doc:
//		Check if a given child is a scalar expression/group
//
//---------------------------------------------------------------------------
BOOL
CExpressionHandle::FScalarChild(ULONG child_index) const
{
	if (NULL != Pexpr())
	{
		return (*Pexpr())[child_index]->Pop()->FScalar();
	}

	GPOS_ASSERT(NULL != Pgexpr());

	return (*Pgexpr())[child_index]->FScalar();
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Arity
//
//	@doc:
//		Return number of children of attached expression/group expression
//
//---------------------------------------------------------------------------
ULONG
CExpressionHandle::Arity() const
{
	if (NULL != Pexpr())
	{
		return Pexpr()->Arity();
	}

	GPOS_ASSERT(NULL != Pgexpr());

	return Pgexpr()->Arity();
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::UlLastNonScalarChild
//
//	@doc:
//		Return the index of the last non-scalar child. This is only valid if
//		Arity() is greater than 0
//
//---------------------------------------------------------------------------
ULONG
CExpressionHandle::UlLastNonScalarChild() const
{
	const ULONG arity = Arity();
	if (0 == arity)
	{
		return gpos::ulong_max;
	}

	ULONG ulLastNonScalarChild = arity - 1;
	while (0 < ulLastNonScalarChild && FScalarChild(ulLastNonScalarChild))
	{
		ulLastNonScalarChild--;
	}

	if (!FScalarChild(ulLastNonScalarChild))
	{
		// we need to check again that index points to a non-scalar child
		// since operator's children may be all scalar (e.g. index-scan)
		return ulLastNonScalarChild;
	}

	return gpos::ulong_max;
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::UlFirstNonScalarChild
//
//	@doc:
//		Return the index of the first non-scalar child. This is only valid if
//		Arity() is greater than 0
//
//---------------------------------------------------------------------------
ULONG
CExpressionHandle::UlFirstNonScalarChild() const
{
	const ULONG arity = Arity();
	if (0 == arity)
	{
		return gpos::ulong_max;
	}

	ULONG ulFirstNonScalarChild = 0;
	while (ulFirstNonScalarChild < arity - 1 &&
		   FScalarChild(ulFirstNonScalarChild))
	{
		ulFirstNonScalarChild++;
	}

	if (!FScalarChild(ulFirstNonScalarChild))
	{
		// we need to check again that index points to a non-scalar child
		// since operator's children may be all scalar (e.g. index-scan)
		return ulFirstNonScalarChild;
	}

	return gpos::ulong_max;
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::UlNonScalarChildren
//
//	@doc:
//		Return number of non-scalar children
//
//---------------------------------------------------------------------------
ULONG
CExpressionHandle::UlNonScalarChildren() const
{
	const ULONG arity = Arity();
	ULONG ulNonScalarChildren = 0;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (!FScalarChild(ul))
		{
			ulNonScalarChildren++;
		}
	}

	return ulNonScalarChildren;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Prpp
//
//	@doc:
//		Retrieve required relational props of n-th child;
//		Assumes caller knows what properties to ask for;
//
//---------------------------------------------------------------------------
CReqdPropPlan *
CExpressionHandle::Prpp(ULONG child_index) const
{
	GPOS_ASSERT(child_index < m_pdrgprp->Size());

	CReqdProp *prp = (*m_pdrgprp)[child_index];
	GPOS_ASSERT(prp->FPlan() && "Unexpected property type");

	return CReqdPropPlan::Prpp(prp);
}


//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Pop
//
//	@doc:
//		Get operator from handle
//
//---------------------------------------------------------------------------
COperator *
CExpressionHandle::Pop() const
{
	if (NULL != m_pexpr)
	{
		GPOS_ASSERT(NULL == m_pgexpr);

		return m_pexpr->Pop();
	}

	if (NULL != m_pgexpr)
	{
		return m_pgexpr->Pop();
	}

	GPOS_ASSERT(!"Handle was not attached properly");
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CExpressionHandle::Pop
//
//	@doc:
//		Get child operator from handle
//
//---------------------------------------------------------------------------
COperator *
CExpressionHandle::Pop(ULONG child_index) const
{
	GPOS_ASSERT(child_index < Arity());

	if (NULL != m_pexpr)
	{
		GPOS_ASSERT(NULL == m_pgexpr);

		return (*m_pexpr)[child_index]->Pop();
	}

//	if (NULL != m_pcc)
//	{
//		COptimizationContext *pocChild = (*m_pcc->Pdrgpoc())[child_index];
//		GPOS_ASSERT(NULL != pocChild);
//
//		CCostContext *pccChild = pocChild->PccBest();
//		GPOS_ASSERT(NULL != pccChild);
//
//		return pccChild->Pgexpr()->Pop();
//	}

	return NULL;
}

COperator *
CExpressionHandle::PopGrandchild(ULONG child_index, ULONG grandchild_index,
								 CCostContext **grandchildContext) const
{
	GPOS_ASSERT(child_index < Arity());

	if (grandchildContext)
	{
		*grandchildContext = NULL;
	}

	if (NULL != m_pexpr)
	{
		GPOS_ASSERT(NULL == m_pcc);

		CExpression *childExpr = (*m_pexpr)[child_index];

		if (NULL != childExpr)
		{
			return (*childExpr)[grandchild_index]->Pop();
		}

		return NULL;
	}

	if (NULL != m_pcc)
	{
		COptimizationContext *pocChild = (*m_pcc->Pdrgpoc())[child_index];
		GPOS_ASSERT(NULL != pocChild);

//		CCostContext *pccChild = pocChild->PccBest();
//		GPOS_ASSERT(NULL != pccChild);
//
//		COptimizationContext *pocGrandchild =
//			(*pccChild->Pdrgpoc())[grandchild_index];
//
//		if (NULL != pocGrandchild)
//		{
//			CCostContext *pccgrandchild = pocGrandchild->PccBest();
//
//			if (grandchildContext)
//			{
//				*grandchildContext = pccgrandchild;
//			}
//
//			return pccgrandchild->Pgexpr()->Pop();
//		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
// CExpressionHandle::PexprScalarRepChild
//
// Get a representative (inexact) scalar child at given index. Subqueries
// in the child are replaced by a TRUE or NULL constant. Use this method
// where exactness is not required, e. g. for statistics derivation,
// costing, or for heuristics.
//
//---------------------------------------------------------------------------
CExpression *
CExpressionHandle::PexprScalarRepChild(ULONG child_index) const
{
	GPOS_ASSERT(child_index < Arity());

	if (NULL != m_pgexpr)
	{
		// access scalar expression cached on the child scalar group
		GPOS_ASSERT((*m_pgexpr)[child_index]->FScalar());

		CExpression *pexprScalar = (*m_pgexpr)[child_index]->PexprScalarRep();
		GPOS_ASSERT(NULL != pexprScalar);

		return pexprScalar;
	}

	if (NULL != m_pexpr && NULL != (*m_pexpr)[child_index]->Pgexpr())
	{
		// if the expression does not come from a group, but its child does then
		// get the scalar child from that group
		CGroupExpression *pgexpr = (*m_pexpr)[child_index]->Pgexpr();
		CExpression *pexprScalar = pgexpr->Pgroup()->PexprScalarRep();
		GPOS_ASSERT(NULL != pexprScalar);

		return pexprScalar;
	}

	// access scalar expression from the child expression node
	GPOS_ASSERT((*m_pexpr)[child_index]->Pop()->FScalar());

	return (*m_pexpr)[child_index];
}


//---------------------------------------------------------------------------
// CExpressionHandle::PexprScalarRep
//
// Get a representative scalar expression attached to handle,
// return NULL if handle is not attached to a scalar expression.
// Note that this may be inexact if handle is attached to a
// CGroupExpression - subqueries will be replaced by a TRUE or NULL
// constant. Use this method where exactness is not required, e. g.
// for statistics derivation, costing, or for heuristics.
//
//---------------------------------------------------------------------------
CExpression *
CExpressionHandle::PexprScalarRep() const
{
	if (!Pop()->FScalar())
	{
		return NULL;
	}

	if (NULL != m_pexpr)
	{
		return m_pexpr;
	}

	if (NULL != m_pgexpr)
	{
		return m_pgexpr->Pgroup()->PexprScalarRep();
	}

	return NULL;
}


// return an exact scalar child at given index or return null if not possible
// (use this where exactness is required, e.g. for constraint derivation)
CExpression *
CExpressionHandle::PexprScalarExactChild(ULONG child_index,
										 BOOL error_on_null_return) const
{
	CExpression *result_expr = NULL;
	if (NULL != m_pgexpr && !(*m_pgexpr)[child_index]->FScalarRepIsExact())
	{
		result_expr = NULL;
	}

	else if (NULL != m_pexpr && NULL != (*m_pexpr)[child_index]->Pgexpr() &&
			 !((*m_pexpr)[child_index]
				   ->Pgexpr()
				   ->Pgroup()
				   ->FScalarRepIsExact()))
	{
		// the expression does not come from a group, but its child does and
		// the child group does not have an exact expression
		result_expr = NULL;
	}

	else
	{
		result_expr = PexprScalarRepChild(child_index);
	}
	if (NULL == result_expr && error_on_null_return)
	{
		GPOS_RAISE(CException::ExmaInvalid, CException::ExmiInvalid,
				   GPOS_WSZ_LIT("Generated invalid plan with subquery"));
	}
	return result_expr;
}

// return an exact scalar expression attached to handle or null if not possible
// (use this where exactness is required, e.g. for constraint derivation)
CExpression *
CExpressionHandle::PexprScalarExact() const
{
	if (NULL != m_pgexpr && !m_pgexpr->Pgroup()->FScalarRepIsExact())
	{
		return NULL;
	}

	return PexprScalarRep();
}

// EOF
