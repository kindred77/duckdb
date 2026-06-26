//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCTEInfo.h
//
//	@doc:
//		Information about CTEs in a query
//---------------------------------------------------------------------------
#ifndef GPOPT_CCTEInfo_H
#define GPOPT_CCTEInfo_H

#include "gpos/base.h"
#include "gpos/common/CHashMap.h"
#include "gpos/common/CStack.h"

#include "gpopt/base/CColRefSet.h"
#include "gpopt/base/CColumnFactory.h"
#include "gpopt/operators/CExpression.h"

namespace gpopt
{
// fwd declarations
class CLogicalCTEConsumer;

// hash map: CColRef -> GP_ULONG
typedef CHashMap<CColRef, GP_ULONG, CColRef::HashValue, CColRef::Equals,
				 CleanupNULL<CColRef>, CleanupDelete<GP_ULONG> >
	ColRefToUlongMap;

//---------------------------------------------------------------------------
//	@class:
//		CCTEInfo
//
//	@doc:
//		Global information about common table expressions (CTEs) including:
//		- the expression tree that defines each CTE
//		- the number of consumers created by the optimizer
//		- a mapping from consumer columns to producer columns
//
//---------------------------------------------------------------------------
class CCTEInfo : public CRefCount
{
private:
	//-------------------------------------------------------------------
	//	@struct:
	//		SConsumerCounter
	//
	//	@doc:
	//		Representation of the number of consumers of a given CTE inside
	// 		a specific context (e.g. inside the main query, inside another CTE, etc.)
	//
	//-------------------------------------------------------------------
	struct SConsumerCounter
	{
	private:
		// consumer ID
		GP_ULONG m_ulCTEId;

		// number of occurrences
		GP_ULONG m_ulCount;

	public:
		// ctor
		explicit SConsumerCounter(GP_ULONG ulCTEId)
			: m_ulCTEId(ulCTEId), m_ulCount(1)
		{
		}

		// consumer ID
		GP_ULONG
		UlCTEId() const
		{
			return m_ulCTEId;
		}

		// number of consumers
		GP_ULONG
		UlCount() const
		{
			return m_ulCount;
		}

		// increment number of consumers
		void
		Increment()
		{
			m_ulCount++;
		}
	};

	// hash map mapping GP_ULONG -> SConsumerCounter
	typedef CHashMap<GP_ULONG, SConsumerCounter, gpos::HashValue<GP_ULONG>,
					 gpos::Equals<GP_ULONG>, CleanupDelete<GP_ULONG>,
					 CleanupDelete<SConsumerCounter> >
		UlongToConsumerCounterMap;

	// map iterator
	typedef CHashMapIter<GP_ULONG, SConsumerCounter, gpos::HashValue<GP_ULONG>,
						 gpos::Equals<GP_ULONG>, CleanupDelete<GP_ULONG>,
						 CleanupDelete<SConsumerCounter> >
		UlongToConsumerCounterMapIter;

	// hash map mapping GP_ULONG -> UlongToConsumerCounterMap: maps from CTE producer ID to all consumers inside this CTE
	typedef CHashMap<GP_ULONG, UlongToConsumerCounterMap, gpos::HashValue<GP_ULONG>,
					 gpos::Equals<GP_ULONG>, CleanupDelete<GP_ULONG>,
					 CleanupRelease<UlongToConsumerCounterMap> >
		UlongToProducerConsumerMap;

	//-------------------------------------------------------------------
	//	@struct:
	//		CCTEInfoEntry
	//
	//	@doc:
	//		A single entry for CTEInfo, representing a single CTE producer
	//
	//-------------------------------------------------------------------
	class CCTEInfoEntry : public CRefCount
	{
	private:
		// memory pool
		CMemoryPool *m_mp;

		// logical producer expression
		CExpression *m_pexprCTEProducer;

		// map columns of all created consumers of current CTE to their positions in consumer output
		ColRefToUlongMap *m_phmcrulConsumers;

		// is this CTE used
		GP_BOOL m_fUsed;

	public:
		// ctors
		CCTEInfoEntry(CMemoryPool *mp, CExpression *pexprCTEProducer);
		CCTEInfoEntry(CMemoryPool *mp, CExpression *pexprCTEProducer,
					  GP_BOOL fUsed);

		// dtor
		~CCTEInfoEntry();

		// CTE expression
		CExpression *
		Pexpr() const
		{
			return m_pexprCTEProducer;
		}

		// is this CTE used?
		GP_BOOL
		FUsed() const
		{
			return m_fUsed;
		}

		// CTE id
		GP_ULONG UlCTEId() const;

		// mark CTE as unused
		void
		MarkUnused()
		{
			m_fUsed = false;
		}

		// add given columns to consumers column map
		void AddConsumerCols(CColRefArray *colref_array);

		// return position of given consumer column in consumer output
		GP_ULONG UlConsumerColPos(CColRef *colref);

	};	//class CCTEInfoEntry

	// hash maps mapping GP_ULONG -> CCTEInfoEntry
	typedef CHashMap<GP_ULONG, CCTEInfoEntry, gpos::HashValue<GP_ULONG>,
					 gpos::Equals<GP_ULONG>, CleanupDelete<GP_ULONG>,
					 CleanupRelease<CCTEInfoEntry> >
		UlongToCTEInfoEntryMap;

	// map iterator
	typedef CHashMapIter<GP_ULONG, CCTEInfoEntry, gpos::HashValue<GP_ULONG>,
						 gpos::Equals<GP_ULONG>, CleanupDelete<GP_ULONG>,
						 CleanupRelease<CCTEInfoEntry> >
		UlongToCTEInfoEntryMapIter;

	// memory pool
	CMemoryPool *m_mp;

	// mapping from cte producer id -> cte info entry
	UlongToCTEInfoEntryMap *m_phmulcteinfoentry;

	// next available CTE Id
	GP_ULONG m_ulNextCTEId;

	// whether or not to inline CTE consumers
	GP_BOOL m_fEnableInlining;

	// consumers inside each cte/main query
	UlongToProducerConsumerMap *m_phmulprodconsmap;

	// initialize default statistics for a given CTE Producer
	void InitDefaultStats(CExpression *pexprCTEProducer);

	// preprocess CTE producer expression
	CExpression *PexprPreprocessCTEProducer(
		const CExpression *pexprCTEProducer);

	// private copy ctor
	CCTEInfo(const CCTEInfo &);

	// number of consumers of given CTE inside a given parent
	GP_ULONG UlConsumersInParent(GP_ULONG ulConsumerId, GP_ULONG ulParentId) const;

	// find all CTE consumers inside given parent, and push them to the given stack
	void FindConsumersInParent(GP_ULONG ulParentId, CBitSet *pbsUnusedConsumers,
							   CStack<GP_ULONG> *pstack);

public:
	// ctor
	explicit CCTEInfo(CMemoryPool *mp);

	//dtor
	virtual ~CCTEInfo();

	// logical cte producer with given id
	CExpression *PexprCTEProducer(GP_ULONG ulCTEId) const;

	// number of CTE consumers of given CTE
	GP_ULONG UlConsumers(GP_ULONG ulCTEId) const;

	// check if given CTE is used
	GP_BOOL FUsed(GP_ULONG ulCTEId) const;

	// increment number of CTE consumers
	void IncrementConsumers(GP_ULONG ulConsumerId,
							GP_ULONG ulParentCTEId = gpos::ulong_max);

	// add cte producer to hashmap
	void AddCTEProducer(CExpression *pexprCTEProducer);

	// replace cte producer with given expression
	void ReplaceCTEProducer(CExpression *pexprCTEProducer);

	// next available CTE id
	GP_ULONG
	next_id()
	{
		return m_ulNextCTEId++;
	}

	// derive the statistics on the CTE producer
	void DeriveProducerStats(
		CLogicalCTEConsumer *popConsumer,  // CTE Consumer operator
		CColRefSet *pcrsStat  // required stat columns on the CTE Consumer
	);

	// return a CTE requirement with all the producers as optional
	CCTEReq *PcterProducers(CMemoryPool *mp) const;

	// return an array of all stored CTE expressions
	CExpressionArray *PdrgPexpr(CMemoryPool *mp) const;

	// disable CTE inlining
	void
	DisableInlining()
	{
		m_fEnableInlining = false;
	}

	// whether or not to inline CTE consumers
	GP_BOOL
	FEnableInlining() const
	{
		return m_fEnableInlining;
	}

	// mark unused CTEs
	void MarkUnusedCTEs();

	// walk the producer expressions and add the mapping between computed column
	// and their corresponding used column(s)
	void MapComputedToUsedCols(CColumnFactory *col_factory) const;

	// add given columns to consumers column map
	void AddConsumerCols(GP_ULONG ulCTEId, CColRefArray *colref_array);

	// return position of given consumer column in consumer output
	GP_ULONG UlConsumerColPos(GP_ULONG ulCTEId, CColRef *colref);

	// return a map from Id's of consumer columns in the given column set to their corresponding producer columns
	UlongToColRefMap *PhmulcrConsumerToProducer(CMemoryPool *mp, GP_ULONG ulCTEId,
												CColRefSet *pcrs,
												CColRefArray *pdrgpcrProducer);

};	// CCTEInfo
}  // namespace gpopt

#endif	// !GPOPT_CCTEInfo_H

// EOF
