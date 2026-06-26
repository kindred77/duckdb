//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CJobTest.h
//
//	@doc:
//		Job implementation for testing purposes
//---------------------------------------------------------------------------
#ifndef GPOPT_CJobTest_H
#define GPOPT_CJobTest_H

#include "gpos/base.h"

#include "gpopt/search/CJob.h"
#include "gpopt/search/CJobQueue.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CJobTest
//
//	@doc:
//		Job derivative for unittests
//
//---------------------------------------------------------------------------
class CJobTest : public CJob
{
	// friends
	friend class CJobFactory;

public:
	// job test type
	enum ETestType
	{
		EttSpawn,
		EttStartQueue,
		EttQueueu
	};

private:
	// test type
	ETestType m_ett;

	// number of job spawning rounds
	GP_ULONG m_ulRounds;

	// spawning fanout
	GP_ULONG m_ulFanout;

	// CPU-burning iterations per job
	GP_ULONG m_ulIters;

	// iteration counter
	static ULONG_PTR m_ulpCnt;

	// job queue
	CJobQueue *m_pjq;

	// test job spawning
	GP_BOOL FSpawn(CSchedulerContext *psc);

	// start jobs to be queued
	GP_BOOL FStartQueue(CSchedulerContext *psc);

	// test job queueing
	GP_BOOL FQueue(CSchedulerContext *psc);

	// burn some CPU to simulate actual work
	void Loop();

public:
	// ctor
	CJobTest();

	// dtor
	virtual ~CJobTest();

	// execution
	virtual GP_BOOL FExecute(CSchedulerContext *psc);

#ifdef GPOS_DEBUG
	// printer
	virtual IOstream &OsPrint(IOstream &) const;
#endif	// GPOS_DEBUG

	// set execution parameters
	void
	Init(ETestType ett, GP_ULONG ulRounds, GP_ULONG ulFanout, GP_ULONG ulIters,
		 CJobQueue *pjq)
	{
		m_ett = ett;
		m_ulRounds = ulRounds;
		m_ulFanout = ulFanout;
		m_ulIters = ulIters;
		m_pjq = pjq;
	}

	// copy execution parameters
	void
	Init(CJobTest *pjt)
	{
		Init(pjt->m_ett, pjt->m_ulRounds, pjt->m_ulFanout, pjt->m_ulIters,
			 pjt->m_pjq);
	}

	// reset
	static void
	ResetCnt()
	{
		m_ulpCnt = 0;
	}

	// conversion function
	static CJobTest *
	PjConvert(CJob *pj)
	{
		GPOS_ASSERT(NULL != pj);
		GPOS_ASSERT(EjtTest == pj->Ejt());

		return dynamic_cast<CJobTest *>(pj);
	}
};
}  // namespace gpopt

#endif	// !GPOPT_CJobTest_H


// EOF
