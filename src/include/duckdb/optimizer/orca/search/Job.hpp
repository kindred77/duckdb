
#pragma once

namespace duckdb {

class Job;
using JobPtr = std::shared_ptr<Job>;

class Job {
    // friends
	friend class JobFactory;
	friend class JobQueue;
	friend class Scheduler;

public:
    // job type
	enum EJobType
	{
		EjtTest = 0,
		EjtGroupOptimization,
		EjtGroupImplementation,
		EjtGroupExploration,
		EjtGroupExpressionOptimization,
		EjtGroupExpressionImplementation,
		EjtGroupExpressionExploration,
		EjtTransformation,

		EjtInvalid,
		EjtSentinel = EjtInvalid
	};

private:
    JobPtr m_pjParent{nullptr};

	// assigned job queue
	JobQueuePtr m_pjq{nullptr};

	// reference counter
	size_t m_ulpRefs{0};

	// job id - set by job factory
	uint32_t m_id{0};

	// job type
	EJobType m_ejt;

	// flag indicating if job is initialized
	bool m_fInit{false};

	// set type
	void
	SetJobType(EJobType ejt)
	{
		m_ejt = ejt;
	}

	//-------------------------------------------------------------------
	// Interface for CScheduler
	//-------------------------------------------------------------------

	// parent accessor
	JobPtr
	PjParent() const
	{
		return m_pjParent;
	}

	// set parent
	void
	SetParent(JobPtr pj)
	{
		//GPOS_ASSERT(this != pj);

		m_pjParent = pj;
	}

	// increment reference counter
	void
	IncRefs()
	{
		m_ulpRefs++;
	}

	// decrement reference counter
	uintptr_t
	UlpDecrRefs()
	{
		//GPOS_ASSERT(0 < m_ulpRefs && "Decrement counter from 0");
		return m_ulpRefs--;
	}

	// notify parent of job completion;
	// return true if parent is runnable;
	bool FResumeParent() const;

protected:
	// id accessor
	uint32_t
	Id() const
	{
		return m_id;
	}

	// ctor
	Job() = default;

	// dtor
	virtual ~Job()
	{
		//GPOS_ASSERT_IMP(!ITask::Self()->HasPendingExceptions(), 0 == m_ulpRefs);
	}

	// reset job
	virtual void Reset();

	// check if job is initialized
	bool
	FInit() const
	{
		return m_fInit;
	}

	// mark job as initialized
	void
	SetInit()
	{
		//GPOS_ASSERT(false == m_fInit);

		m_fInit = true;
	}

public:
    Job(const Job &) = delete;

	// actual job execution given a scheduling context
	// returns true if job completes, false if it is suspended
	virtual bool FExecute(SchedulerContextPtr psc) = 0;

	// type accessor
	EJobType
	Ejt() const
	{
		return m_ejt;
	}

	// job queue accessor
	JobQueuePtr
	Pjq() const
	{
		return m_pjq;
	}

	// set job queue
	void
	SetJobQueue(JobQueuePtr pjq)
	{
		//GPOS_ASSERT(nullptr != pjq);
		m_pjq = pjq;
	}

	// cleanup internal state
	virtual void
	Cleanup()
	{
	}

	// link for job queueing
	SLink m_linkQueue;
};

}