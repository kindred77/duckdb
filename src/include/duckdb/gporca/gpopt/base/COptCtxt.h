//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		COptCtxt.h
//
//	@doc:
//		Optimizer context object; contains all global objects pertaining to
//		one optimization
//---------------------------------------------------------------------------
#ifndef GPOPT_COptCtxt_H
#define GPOPT_COptCtxt_H

#include "naucrates/traceflags/traceflags.h"
#include "gpos/base.h"
#include "gpos/task/CTaskLocalStorageObject.h"

#include "gpopt/base/CCTEInfo.h"
#include "gpopt/base/CColumnFactory.h"
#include "gpopt/base/IComparator.h"
#include "gpopt/mdcache/CMDAccessor.h"

namespace gpopt
{
using namespace gpos;

// forward declarations
class CColRefSet;
class COptimizerConfig;
class ICostModel;
class IConstExprEvaluator;

//---------------------------------------------------------------------------
//	@class:
//		COptCtxt
//
//	@doc:
//		"Optimizer Context" is a container of global objects (mostly
//		singletons) that are needed by the optimizer.
//
//		A COptCtxt object is instantiated in COptimizer::PdxlnOptimize() via
//		COptCtxt::PoctxtCreate() and stored as a task local object. The global
//		information contained in it can be accessed by calling
//		COptCtxt::PoctxtFromTLS(), instead of passing a pointer to it all
//		around. For example to get the global CMDAccessor:
//			CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
//
//---------------------------------------------------------------------------
class COptCtxt : public CTaskLocalStorageObject
{
private:
	// private copy ctor
	COptCtxt(COptCtxt &);

	// shared memory pool
	CMemoryPool *m_mp;

	// column factory
	CColumnFactory *m_pcf;

	// metadata accessor;
	CMDAccessor *m_pmda;

	// cost model
	ICostModel *m_cost_model;

	// constant expression evaluator
	IConstExprEvaluator *m_pceeval;

	// comparator between IDatum instances
	IComparator *m_pcomp;

	// atomic counter for generating part index ids
	GP_ULONG m_auPartId;

	// global CTE information
	CCTEInfo *m_pcteinfo;

	// system columns required in query output
	CColRefArray *m_pdrgpcrSystemCols;

	// optimizer configurations
	COptimizerConfig *m_optimizer_config;

	// whether or not we are optimizing a DML query
	GP_BOOL m_fDMLQuery;

	// value for the first valid part id
	static GP_ULONG m_ulFirstValidPartId;

	// if there are master only tables in the query
	GP_BOOL m_has_master_only_tables;

	// does the query contain any volatile functions or
	// functions that read/modify SQL data
	GP_BOOL m_has_volatile_func;

	// does the query have replicated tables
	GP_BOOL m_has_replicated_tables;

	// does this plan have a direct dispatchable filter
	CExpressionArray *m_direct_dispatchable_filters;

public:
	// ctor
	COptCtxt(CMemoryPool *mp, CColumnFactory *col_factory,
			 CMDAccessor *md_accessor, IConstExprEvaluator *pceeval,
			 COptimizerConfig *optimizer_config);

	// dtor
	virtual ~COptCtxt();

	// memory pool accessor
	CMemoryPool *
	Pmp() const
	{
		return m_mp;
	}

	// optimizer configurations
	COptimizerConfig *
	GetOptimizerConfig() const
	{
		return m_optimizer_config;
	}

	// are we optimizing a DML query
	GP_BOOL
	FDMLQuery() const
	{
		return m_fDMLQuery;
	}

	// set the DML flag
	void
	MarkDMLQuery(GP_BOOL fDMLQuery)
	{
		m_fDMLQuery = fDMLQuery;
	}

	void
	SetHasMasterOnlyTables()
	{
		m_has_master_only_tables = true;
	}

	void
	SetHasVolatileFunc()
	{
		m_has_volatile_func = true;
	}

	void
	SetHasReplicatedTables()
	{
		m_has_replicated_tables = true;
	}

	void
	AddDirectDispatchableFilterCandidate(CExpression *filter_expression)
	{
		filter_expression->AddRef();
		m_direct_dispatchable_filters->Append(filter_expression);
	}

	GP_BOOL
	HasMasterOnlyTables() const
	{
		return m_has_master_only_tables;
	}

	GP_BOOL
	HasVolatileFunc() const
	{
		return m_has_volatile_func;
	}

	GP_BOOL
	HasReplicatedTables() const
	{
		return m_has_replicated_tables;
	}

	CExpressionArray *
	GetDirectDispatchableFilters() const
	{
		return m_direct_dispatchable_filters;
	}

	GP_BOOL
	OptimizeDMLQueryWithSingletonSegment() const
	{
		// A DML statement can be optimized by enforcing a gather motion on segment instead of master,
		// whenever a singleton execution is needed.
		// This optmization can not be applied if the query contains any of the following:
		// (1). master-only tables
		// (2). a volatile function
		return !GPOS_FTRACE(EopttraceDisableNonMasterGatherForDML) &&
			   FDMLQuery() && !HasMasterOnlyTables() && !HasVolatileFunc();
	}

	// column factory accessor
	CColumnFactory *
	Pcf() const
	{
		return m_pcf;
	}

	// metadata accessor
	CMDAccessor *
	Pmda() const
	{
		return m_pmda;
	}

	// cost model accessor
	ICostModel *
	GetCostModel() const
	{
		return m_cost_model;
	}

	// constant expression evaluator
	IConstExprEvaluator *
	Pceeval()
	{
		return m_pceeval;
	}

	// comparator
	const IComparator *
	Pcomp()
	{
		return m_pcomp;
	}

	// cte info
	CCTEInfo *
	Pcteinfo()
	{
		return m_pcteinfo;
	}

	// return a new part index id
	GP_ULONG
	UlPartIndexNextVal()
	{
		return m_auPartId++;
	}

	// required system columns
	CColRefArray *
	PdrgpcrSystemCols() const
	{
		return m_pdrgpcrSystemCols;
	}

	// set required system columns
	void
	SetReqdSystemCols(CColRefArray *pdrgpcrSystemCols)
	{
		GPOS_ASSERT(NULL != pdrgpcrSystemCols);

		CRefCount::SafeRelease(m_pdrgpcrSystemCols);
		m_pdrgpcrSystemCols = pdrgpcrSystemCols;
	}

	// factory method
	static COptCtxt *PoctxtCreate(CMemoryPool *mp, CMDAccessor *md_accessor,
								  IConstExprEvaluator *pceeval,
								  COptimizerConfig *optimizer_config);

	// shorthand to retrieve opt context from TLS
	inline static COptCtxt *
	PoctxtFromTLS()
	{
		return reinterpret_cast<COptCtxt *>(
			ITask::Self()->GetTls().Get(CTaskLocalStorage::EtlsidxOptCtxt));
	}

	// return true if all enforcers are enabled
	static GP_BOOL FAllEnforcersEnabled();

};	// class COptCtxt
}  // namespace gpopt


#endif	// !GPOPT_COptCtxt_H

// EOF
