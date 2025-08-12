//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC Corp.
//
//	@filename:
//		CGroupExpression.h
//
//	@doc:
//		Equivalent of CExpression inside Memo structure
//---------------------------------------------------------------------------
#ifndef DUCKDB_CGROUPEXPRESSION_H
#define DUCKDB_CGROUPEXPRESSION_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"

#include "gpopt/operators/COperator.h"
#include "gpopt/search/CGroup.h"
#include "gpopt/xforms/CXform.h"

#define GPOPT_INVALID_GEXPR_ID gpos::ulong_max

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CGroupExpression
//
//	@doc:
//		Expression representation inside Memo structure
//
//---------------------------------------------------------------------------
class CGroupExpression : public CRefCount,
						 public DbgPrintMixin<CGroupExpression>
{
public:
#ifdef GPOS_DEBUG
	// debug print; for interactive debugging sessions only
	void DbgPrintWithProperties();
#endif	// GPOS_DEBUG

	// states of a group expression
	enum EState
	{
		estUnexplored,	// initial state

		estExploring,  // ongoing exploration
		estExplored,   // done exploring

		estImplementing,  // ongoing implementation
		estImplemented,	  // done implementing

		estSentinel
	};

	// circular dependency state
	enum ECircularDependency
	{
		ecdDefault,				// default state
		ecdCircularDependency,	// contains circular dependency

		ecdSentinel
	};

private:


	// expression id
	ULONG m_id;

	// duplicate group expression
	CGroupExpression *m_pgexprDuplicate;

	// operator class
	COperator *m_pop;

	// array of child groups
	CGroupArray *m_pdrgpgroup;

	// sorted array of children groups for faster comparison
	// of order-insensitive operators
	CGroupArray *m_pdrgpgroupSorted;

	// back pointer to group
	CGroup *m_pgroup;

	// id of xform that generated group expression
	CXform::EXformId m_exfidOrigin;

	// group expression that generated current group expression via xform
	CGroupExpression *m_pgexprOrigin;

	// flag to indicate if group expression was created as a node at some
	// intermediate level when origin expression was inserted to memo
	BOOL m_fIntermediate;

	// state of group expression
	EState m_estate;

	// optimization level
	EOptimizationLevel m_eol;

	// circular dependency state
	ECircularDependency m_ecirculardependency;

	// set group back pointer
	void SetGroup(CGroup *pgroup);

	// set group expression id
	void SetId(ULONG id);

	// postprocessing after applying transformation
	void PostprocessTransform(CMemoryPool *pmpLocal, CMemoryPool *pmpGlobal,
							  CXform *pxform);

	// private copy ctor
	CGroupExpression(const CGroupExpression &);

	//private dummy ctor; used for creating invalid gexpr
	CGroupExpression()
		: m_id(GPOPT_INVALID_GEXPR_ID),
		  m_pop(NULL),
		  m_pdrgpgroup(NULL),
		  m_pdrgpgroupSorted(NULL),
		  m_pgroup(NULL),
		  m_exfidOrigin(CXform::ExfInvalid),
		  m_pgexprOrigin(NULL),
		  m_fIntermediate(false),
		  m_estate(estUnexplored),
		  m_eol(EolLow){};


public:
	// ctor
	CGroupExpression(CMemoryPool *mp, COperator *pop, CGroupArray *pdrgpgroup,
					 CXform::EXformId exfid, CGroupExpression *pgexprOrigin,
					 BOOL fIntermediate);

	// dtor
	~CGroupExpression();

	// duplicate group expression accessor
	CGroupExpression *
	PgexprDuplicate() const
	{
		return m_pgexprDuplicate;
	}

	// set duplicate group expression
	void
	SetDuplicate(CGroupExpression *pgexpr)
	{
		GPOS_ASSERT(NULL != pgexpr);

		m_pgexprDuplicate = pgexpr;
	}

	// initialize group expression
	void Init(CGroup *pgroup, ULONG id);

	// reset group expression
	void
	Reset(CGroup *pgroup, ULONG id)
	{
		m_pgroup = pgroup;
		m_id = id;
	}

	// optimization level accessor
	EOptimizationLevel
	Eol() const
	{
		return m_eol;
	}

	// shorthand to access children
	CGroup *
	operator[](ULONG ulPos) const
	{
		GPOS_ASSERT(NULL != m_pdrgpgroup);

		CGroup *pgroup = (*m_pdrgpgroup)[ulPos];

		// during optimization, the operator returns the duplicate group;
		// in exploration and implementation the group may contain
		// group expressions that have not been processed yet;
		if (0 == pgroup->UlGExprs())
		{
			GPOS_ASSERT(pgroup->FDuplicateGroup());
			return pgroup->PgroupDuplicate();
		}
		return pgroup;
	};

	// arity function
	ULONG
	Arity() const
	{
		return m_pdrgpgroup->Size();
	}

	// accessor for operator
	COperator *
	Pop() const
	{
		return m_pop;
	}

	// accessor for id
	ULONG
	Id() const
	{
		return m_id;
	}

	// accessor for containing group
	CGroup *
	Pgroup() const
	{
		return m_pgroup;
	}

	// origin xform
	CXform::EXformId
	ExfidOrigin() const
	{
		return m_exfidOrigin;
	}

	// origin group expression
	CGroupExpression *
	PgexprOrigin() const
	{
		return m_pgexprOrigin;
	}

	// comparison operator for hashtables
	BOOL
	operator==(const CGroupExpression &gexpr) const
	{
		return gexpr.Matches(this);
	}

	// equality function for hash table
	static BOOL
	Equals(const CGroupExpression &gexprLeft,
		   const CGroupExpression &gexprRight)
	{
		return gexprLeft == gexprRight;
	}

	// match group expression against given operator and its children
	BOOL Matches(const CGroupExpression *) const;

	// transform group expression
	void Transform(CMemoryPool *mp, CMemoryPool *pmpLocal, CXform *pxform,
				   CXformResult *pxfres, ULONG *pulElapsedTime,
				   ULONG *pulNumberOfBindings);

	// check if group expression has been explored
	BOOL
	FExplored() const
	{
		return (estExplored <= m_estate);
	}

	// check if group expression has been implemented
	BOOL
	FImplemented() const
	{
		return (estImplemented == m_estate);
	}

	CGroupArray *
	Pdrgpgroup() const
	{
		return m_pdrgpgroup;
	}

	// link for list in Group
	SLink m_linkGroup;

	// link for group expression hash table
	SLink m_linkMemo;

	// invalid group expression
	static const CGroupExpression m_gexprInvalid;
};	// class CGroupExpression

}  // namespace gpopt


#endif	// !DUCKDB_CGROUPEXPRESSION_H

// EOF
