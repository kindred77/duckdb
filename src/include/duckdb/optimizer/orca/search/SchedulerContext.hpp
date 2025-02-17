#pragma once

#include "duckdb/optimizer/orca/search/JobFactory.hpp"
#include "duckdb/optimizer/orca/search/Scheduler.hpp"
#include "duckdb/optimizer/orca/Engine.hpp"

#define GPOPT_SCHED_CTXT_MEM_POOL_SIZE (64 * 1024 * 1024)

namespace duckdb {

// prototypes
class JobFactory;
class Scheduler;
class Engine;

class SchedulerContext;
using SchedulerContextPtr = std::shared_ptr<SchedulerContext>;

class SchedulerContext {
private:
    // job factory
	JobFactoryPtr m_pjf;

	// scheduler
	SchedulerPtr m_psched{nullptr};

	// optimization engine
	EnginePtr m_peng;

	// flag indicating if context has been initialized
	bool m_fInit{false};

	bool
	FInit() const
	{
		return m_fInit;
	}
public:
	SchedulerContext(const SchedulerContext &) = delete;

	// ctor
	SchedulerContext();

	// dtor
	~SchedulerContext();

	// initialization
	void Init(JobFactoryPtr pjf, SchedulerPtr psched, EnginePtr peng);

	// job factory accessor
	JobFactoryPtr
	Pjf() const
	{
		//GPOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_pjf;
	}

	// scheduler accessor
	SchedulerPtr
	Psched() const
	{
		//GPOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_psched;
	}

	// engine accessor
	EnginePtr
	Peng() const
	{
		//GPOS_ASSERT(FInit() && "Scheduling context is not initialized");
		return m_peng;
	}
};

}
