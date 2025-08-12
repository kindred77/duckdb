//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2011 EMC Corp.
//
//	@filename:
//		CGroup.h
//
//	@doc:
//		Group of equivalent expressions in the Memo structure
//---------------------------------------------------------------------------
#ifndef DUCKDB_CGROUP_H
#define DUCKDB_CGROUP_H

#include "gpos/base.h"
#include "gpos/common/CDynamicPtrArray.h"
#include "gpos/common/CSyncHashtable.h"
#include "gpos/common/CSyncList.h"

#include "gpopt/search/CJobQueue.h"
#include "gpopt/search/CTreeMap.h"

#define GPOPT_INVALID_GROUP_ID gpos::ulong_max

namespace gpopt
{
using namespace gpos;


// forward declarations
class CGroup;
class CGroupExpression;
class CGroupProxy;
class COptimizationContext;
class CExpression;

// type definitions
// array of groups
typedef CDynamicPtrArray<CGroup, CleanupNULL> CGroupArray;

// optimization levels in ascending order,
// under a given optimization context, group expressions in higher levels
// must be optimized before group expressions in lower levels,
// a group expression sets its level in CGroupExpression::SetOptimizationLevel()
enum EOptimizationLevel
{
	EolLow = 0,	 // low optimization level, this is the default level
	EolHigh,	 // high optimization level

	EolSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CGroup
//
//	@doc:
//		Group of equivalent expressions in the Memo structure
//
//---------------------------------------------------------------------------
class CGroup : public CRefCount, public DbgPrintMixin<CGroup>
{
	friend class CGroupProxy;

public:
	// type definition of optimization context hash table
	typedef CSyncHashtable<COptimizationContext,  // entry
						   COptimizationContext /* search key */>
		ShtOC;

	// states of a group
	enum EState
	{
		estUnexplored,	// initial state

		estExploring,  // ongoing exploration
		estExplored,   // done exploring

		estImplementing,  // ongoing implementation
		estImplemented,	  // done implementing

		estOptimizing,	// ongoing optimization
		estOptimized,	// done optimizing

		estSentinel
	};


private:
	// definition of hash table iter
	typedef CSyncHashtableIter<COptimizationContext,  // entry
							   COptimizationContext>
		ShtIter;

	// definition of hash table iter accessor
	typedef CSyncHashtableAccessByIter<COptimizationContext,  // entry
									   COptimizationContext>
		ShtAccIter;

	// definition of hash table accessor
	typedef CSyncHashtableAccessByKey<COptimizationContext,	 // entry
									  COptimizationContext>
		ShtAcc;

	//---------------------------------------------------------------------------
	//	@class:
	//		SContextLink
	//
	//	@doc:
	//		Internal structure to remember processed links in plan enumeration
	//
	//---------------------------------------------------------------------------
	struct SContextLink
	{
	private:
		// cost context in a parent group
		//CCostContext *m_pccParent;

		// index used when treating current group as a child of group expression
		ULONG m_ulChildIndex;

		// optimization context used to locate group expressions in
		// current group to be linked with parent group expression
		COptimizationContext *m_poc;

	public:

		// dtor
		virtual ~SContextLink();

		// hash function
		static ULONG HashValue(const SContextLink *pclink);

		// equality function
		static BOOL Equals(const SContextLink *pclink1,
						   const SContextLink *pclink2);

	};	// struct SContextLink

	// map of processed links in TreeMap structure
	typedef CHashMap<SContextLink, BOOL, SContextLink::HashValue,
					 SContextLink::Equals, CleanupDelete<SContextLink>,
					 CleanupDelete<BOOL> >
		LinkMap;

	// memory pool
	CMemoryPool *m_mp;

	// id is used when printing memo contents
	ULONG m_id;

	// true if group hold scalar expressions
	BOOL m_fScalar;

	// join keys for outer child (only for scalar groups) (used by hash & merge joins)
	CExpressionArray *m_pdrgpexprJoinKeysOuter;

	// join keys for inner child (only for scalar groups) (used by hash & merge joins)
	CExpressionArray *m_pdrgpexprJoinKeysInner;

	// list of group expressions
	CList<CGroupExpression> m_listGExprs;

	// list of duplicate group expressions identified by group merge
	CList<CGroupExpression> m_listDupGExprs;

	// scalar expression for stat derivation (subqueries substituted with a dummy)
	CExpression *m_pexprScalarRep;

	// scalar expression above is exactly the same as the scalar expr in the group
	BOOL m_pexprScalarRepIsExact;

	// pointer to group containing the group expressions
	// of all duplicate groups
	CGroup *m_pgroupDuplicate;

	// map of processed links
	LinkMap *m_plinkmap;

	// hashtable of optimization contexts
	ShtOC m_sht;

	// number of group expressions
	ULONG m_ulGExprs;

	// number of optimization contexts
	ULONG_PTR m_ulpOptCtxts;

	// current state
	EState m_estate;

	// maximum optimization level of member group expressions
	EOptimizationLevel m_eolMax;

	// were new logical operators added to the group?
	BOOL m_fHasNewLogicalOperators;

	// the id of the CTE producer (if any)
	ULONG m_ulCTEProducerId;

	// does the group have any CTE consumer
	BOOL m_fCTEConsumer;

	// exploration job queue
	CJobQueue m_jqExploration;

	// implementation job queue
	CJobQueue m_jqImplementation;

	// private copy ctor
	CGroup(const CGroup &);

	// cleanup optimization contexts on destruction
	void CleanupContexts();

	// increment number of optimization contexts
	ULONG_PTR
	UlpIncOptCtxts()
	{
		return m_ulpOptCtxts++;
	}

	// the following functions are only accessed through group proxy

	// setter of group id
	void SetId(ULONG id);

	// setter of group state
	void SetState(EState estNewState);

	// insert new group expression
	void Insert(CGroupExpression *pgexpr);

	// move duplicate group expression to duplicates list
	void MoveDuplicateGExpr(CGroupExpression *pgexpr);

	// retrieve first group expression
	CGroupExpression *PgexprFirst();

	// retrieve next group expression
	CGroupExpression *PgexprNext(CGroupExpression *pgexpr);

	// print scalar group properties
	IOstream &OsPrintGrpScalarProps(IOstream &os, const CHAR *szPrefix) const;

	// print group properties
	IOstream &OsPrintGrpProps(IOstream &os, const CHAR *szPrefix) const;

	// print group optimization contexts
	IOstream &OsPrintGrpOptCtxts(IOstream &os, const CHAR *szPrefix) const;

public:
	// ctor
	CGroup(CMemoryPool *mp, BOOL fScalar = false);

	// dtor
	~CGroup();

	// id accessor
	ULONG
	Id() const
	{
		return m_id;
	}

	// accessor of maximum optimization level of member group expressions
	EOptimizationLevel
	EolMax() const
	{
		return m_eolMax;
	}

	// does group hold scalar expressions ?
	BOOL
	FScalar() const
	{
		return m_fScalar;
	}

	// join keys of outer child
	CExpressionArray *
	PdrgpexprJoinKeysOuter() const
	{
		return m_pdrgpexprJoinKeysOuter;
	}

	// join keys of inner child
	CExpressionArray *
	PdrgpexprJoinKeysInner() const
	{
		return m_pdrgpexprJoinKeysInner;
	}

	// return a representative cached scalar expression usable for stat derivation etc.
	CExpression *
	PexprScalarRep() const
	{
		return m_pexprScalarRep;
	}

	// is the value returned by PexprScalarRep() exact?
	BOOL
	FScalarRepIsExact() const
	{
		return m_pexprScalarRepIsExact;
	}

	// hash function
	ULONG HashValue() const;

	// number of group expressions accessor
	ULONG
	UlGExprs() const
	{
		return m_ulGExprs;
	}

	// optimization contexts hash table accessor
	ShtOC &
	Sht()
	{
		return m_sht;
	}

	// exploration job queue accessor
	CJobQueue *
	PjqExploration()
	{
		return &m_jqExploration;
	}

	// implementation job queue accessor
	CJobQueue *
	PjqImplementation()
	{
		return &m_jqImplementation;
	}

	// has group been explored?
	BOOL
	FExplored() const
	{
		return estExplored <= m_estate;
	}

	// has group been implemented?
	BOOL
	FImplemented() const
	{
		return estImplemented <= m_estate;
	}

	// has group been optimized?
	BOOL
	FOptimized() const
	{
		return estOptimized <= m_estate;
	}

	// were new logical operators added to the group?
	BOOL
	FHasNewLogicalOperators() const
	{
		return m_fHasNewLogicalOperators;
	}

	// reset has new logical operators flag
	void
	ResetHasNewLogicalOperators()
	{
		m_fHasNewLogicalOperators = false;
	}

	// reset group state
	void ResetGroupState();

	// Check if we need to reset computed stats
	BOOL FResetStats();

	// returns true if stats can be derived on this group
	BOOL FStatsDerivable(CMemoryPool *mp);

	// reset group job queues
	void ResetGroupJobQueues();

	// check if group has duplicates
	BOOL
	FDuplicateGroup() const
	{
		return NULL != m_pgroupDuplicate;
	}

	// duplicate group accessor
	CGroup *
	PgroupDuplicate() const
	{
		return m_pgroupDuplicate;
	}

	// resolve master duplicate group;
	// this is the group that will host all expressions in current group after merging
	void ResolveDuplicateMaster();

	// add duplicate group
	void AddDuplicateGrp(CGroup *pgroup);

	// merge group with its duplicate - not thread-safe
	void MergeGroup();

	// find a context by id
	COptimizationContext *Ppoc(ULONG id) const;

	// insert given context into contexts hash table
	COptimizationContext *PocInsert(COptimizationContext *poc);

	// lookup best expression under given optimization context
	CGroupExpression *PgexprBest(COptimizationContext *poc);

	// materialize a scalar expression for stat derivation if this is a scalar group
	void CreateScalarExpression();

	// materialize a dummy cost context attached to the first group expression
	void CreateDummyCostContext();

	// return the CTE producer ID in the group (if any)
	ULONG
	UlCTEProducerId() const
	{
		return m_ulCTEProducerId;
	}

	// check if there are any CTE producers in the group
	BOOL
	FHasCTEProducer() const
	{
		return (gpos::ulong_max != m_ulCTEProducerId);
	}

	// check if there are any CTE consumers in the group
	BOOL
	FHasAnyCTEConsumer() const
	{
		return m_fCTEConsumer;
	}

	// find group expression with best stats promise and the same given children
	CGroupExpression *PgexprBestPromise(CMemoryPool *mp,
										CGroupExpression *pgexprToMatch);

	// reset link map used in plan enumeration
	void ResetLinkMap();

	// retrieve the group expression containing a CTE Consumer operator
	CGroupExpression *PgexprAnyCTEConsumer();

	// matching of pairs of arrays of groups
	static BOOL FMatchGroups(CGroupArray *pdrgpgroupFst,
							 CGroupArray *pdrgpgroupSnd);

	// matching of pairs of arrays of groups while skipping scalar groups
	static BOOL FMatchNonScalarGroups(CGroupArray *pdrgpgroupFst,
									  CGroupArray *pdrgpgroupSnd);

	// determine if a pair of groups are duplicates
	static BOOL FDuplicateGroups(CGroup *pgroupFst, CGroup *pgroupSnd);

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

	// slink for group list in memo
	SLink m_link;

#ifdef GPOS_DEBUG
	// debug print; for interactive debugging sessions only
	void DbgPrintWithProperties();
#endif

};	// class CGroup

}  // namespace gpopt

#endif	// !DUCKDB_CGROUP_H


// EOF
