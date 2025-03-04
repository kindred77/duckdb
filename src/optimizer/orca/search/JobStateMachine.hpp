#pragma once

#include "duckdb/optimizer/orca/base/StateMachine.hpp"
#include "duckdb/optimizer/orca/search/SchedulerContext.hpp"
#include "duckdb/optimizer/orca/search/Job.hpp"

namespace duckdb {

template <class TEnumState, TEnumState estSentinel, class TEnumEvent,
		  TEnumEvent eevSentinel>
class JobStateMachine {

private:
	// pointer to job action function
	using PFuncAction = TEnumEvent (*)(SchedulerContextPtr, JobPtr);

	// shorthand for state machine
	using SM = StateMachine<TEnumState, estSentinel, TEnumEvent, eevSentinel>;

	// array of actions corresponding to states
	PFuncAction m_rgPfuncAction[estSentinel];

	// job state machine
	SM m_sm;
public:
    JobStateMachine(const JobStateMachine &) = delete;

	// ctor
	JobStateMachine() = default;

	// dtor
	~JobStateMachine() = default;

	// initialize state machine
	void
	Init(const TEnumEvent rgfTransitions[estSentinel][estSentinel]
#ifdef GPOS_DEBUG
		 ,
		 const WCHAR wszStates[estSentinel][GPOPT_FSM_NAME_LENGTH],
		 const WCHAR wszEvent[estSentinel][GPOPT_FSM_NAME_LENGTH]
#endif	// GPOS_DEBUG
	)
	{
		Reset();

		m_sm.Init(rgfTransitions
#ifdef GPOS_DEBUG
				  ,
				  wszStates, wszEvent
#endif	// GPOS_DEBUG
		);
	}

	// match action with state
	void
	SetAction(TEnumState est, PFuncAction pfAction)
	{
		//GPOS_ASSERT(nullptr != pfAction);
		//GPOS_ASSERT(nullptr == m_rgPfuncAction[est] &&
		//			"Action has been already set");

		m_rgPfuncAction[est] = pfAction;
	}

	// run the state machine
	bool
	FRun(SchedulerContextPtr psc, JobPtr pjOwner)
	{
		//GPOS_ASSERT(nullptr != psc);
		//GPOS_ASSERT(nullptr != pjOwner);

		TEnumState estCurrent = estSentinel;
		TEnumState estNext = estSentinel;
		do
		{
			// check if current search stage is timed-out
			if (psc->Peng()->PssCurrent()->FTimedOut())
			{
				// cleanup job state and terminate state machine
				pjOwner->Cleanup();
				return true;
			}

			// find current state
			estCurrent = m_sm.Estate();

			// get the function associated with current state
			PFuncAction pfunc = m_rgPfuncAction[estCurrent];
			//GPOS_ASSERT(nullptr != pfunc);

			// execute the function to get an event
			TEnumEvent eev = pfunc(psc, pjOwner);

			// use the event to transition state machine
			estNext = estCurrent;
			bool fSucceeded /* GPOS_ASSERTS_ONLY */ = m_sm.FTransition(eev, estNext);

			//GPOS_ASSERT(fSucceeded);
		} while (estNext != estCurrent && estNext != m_sm.TesFinal());

		return (estNext == m_sm.TesFinal());
	}

	// reset state machine
	void
	Reset()
	{
		m_sm.Reset();

		// initialize actions array
		for (size_t i = 0; i < estSentinel; i++)
		{
			m_rgPfuncAction[i] = nullptr;
		}
	}
};

}
