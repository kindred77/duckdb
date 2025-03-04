#pragma once

#include "duckdb/optimizer/orca/search/Job.hpp"

namespace duckdb {

class Scheduler;
using SchedulerPtr = std::shared_ptr<Scheduler>;

class Scheduler {
    friend class Job;
private:
    struct SLink {
        // link forward/backward
        void *m_next{nullptr};
        void *m_prev{nullptr};
    };

    // job wrapper; used for inserting job to waiting list (lock-free)
	struct SJobLink {
		// link id, set by sync set
		uint32_t m_id;

		// pointer to job
		JobPtr m_pj;

		// slink for list of waiting jobs
		SLink m_link;

		// initialize link
		void
		Init(JobPtr pj) {
			m_pj = pj;
			m_link.m_prev = m_link.m_next = nullptr;
		}
	};

public:
    // enum for job execution result
	enum EJobResult {
		EjrRunnable = 0,
		EjrSuspended,
		EjrCompleted,

		EjrSentinel
	};

    // ctor
	Scheduler(uint32_t ulJobs
#ifdef GPOS_DEBUG
			   ,
			   BOOL fTrackingJobs = true
#endif	// GPOS_DEBUG
	);

	// dtor
	virtual ~Scheduler();

	// main job processing task
	static void *Run(void *);

	// transition job to completed
	void Complete(JobPtr pj);

	// transition queued job to completed
	void CompleteQueued(JobPtr pj);

	// transition job to suspended
	void Suspend(JobPtr pj);

	// add new job for scheduling
	void Add(JobPtr pj, JobPtr pjParent);

	// resume suspended job
	void Resume(JobPtr pj);

	// print statistics
	void PrintStats() const;

#ifdef GPOS_DEBUG
	// get flag for tracking jobs
	BOOL
	FTrackingJobs() const
	{
		return m_fTrackingJobs;
	}

	// print queue
	IOstream &OsPrintActiveJobs(IOstream &);

#endif	// GPOS_DEBUG

};

}