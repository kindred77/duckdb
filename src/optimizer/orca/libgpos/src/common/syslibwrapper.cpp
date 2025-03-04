//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		syslibwrapper.cpp
//
//	@doc:
//		Wrapper for functions in system library
//
//---------------------------------------------------------------------------

#include "gpos/common/syslibwrapper.h"

#include <sys/stat.h>
#include <sys/time.h>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#else
#include <syslog.h>
#endif

#include "gpos/assert.h"
#include "gpos/error/CException.h"
#include "gpos/utils.h"


using namespace gpos;


//---------------------------------------------------------------------------
//	@function:
//		syslib::GetTimeOfDay
//
//	@doc:
//		Get the date and time
//
//---------------------------------------------------------------------------
void
gpos::syslib::GetTimeOfDay(TIMEVAL *tv, TIMEZONE *tz)
{
	GPOS_ASSERT(NULL != tv);

#ifdef GPOS_DEBUG
	INT res =
#endif	// GPOS_DEBUG
		gettimeofday(tv, tz);

	GPOS_ASSERT(0 == res);
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::GetRusage
//
//	@doc:
//		Get system and user time
//
//---------------------------------------------------------------------------
void
gpos::syslib::GetRusage(RUSAGE *usage)
{
	GPOS_ASSERT(NULL != usage);

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
       FILETIME kernelTime;
       FILETIME userTime;
       [[maybe_unused]] WINBOOL res = !GetProcessTimes(GetCurrentProcess(), NULL, NULL, &kernelTime, &userTime);
       ULARGE_INTEGER ui;
       ui.LowPart = userTime.dwLowDateTime;
       ui.HighPart = userTime.dwHighDateTime;
       ULARGE_INTEGER si;
       si.LowPart = kernelTime.dwLowDateTime;
       si.HighPart = kernelTime.dwHighDateTime;
       usage->ru_stime.tv_usec = si.QuadPart * 100;
       usage->ru_utime.tv_usec = ui.QuadPart * 100;

       // tms buffer;
    // clock_t ticks_per_second = sysconf(CLK_TCK);
       // if (ticks_per_second == -1) {
    //     std::cout << "Error: sysconf" << std::endl;
    //     return;
    // }
       // clock_t start_time = times(&buffer);
    // if (start_time == (clock_t) -1) {
    //     std::cout << "Error: times" << std::endl;
    //     return;
    // }
       // usage->ru_stime.tv_usec = buffer.tms_stime;
       // usage->ru_utime.tv_usec = buffer.tms_utime;
#else

#ifdef GPOS_DEBUG
	INT res =
#endif	// GPOS_DEBUG
		getrusage(RUSAGE_SELF, usage);

#endif
	GPOS_ASSERT(0 == res);
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::OpenLog
//
//	@doc:
//		Open a connection to the system logger for a program
//
//---------------------------------------------------------------------------
void
gpos::syslib::OpenLog(const CHAR *ident, INT option, INT facility)
{

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
       std::cout << "OpenLog not supported yet on WIN." << std::endl;
#else
        openlog(ident, option, facility);
#endif

}


//---------------------------------------------------------------------------
//	@function:
//		syslib::SysLog
//
//	@doc:
//		Generate a log message
//
//---------------------------------------------------------------------------
void
gpos::syslib::SysLog(INT priority, const CHAR *format)
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	std::cout << "SysLog not supported yet on WIN." << std::endl;
#else
    syslog(priority, "%s", format);
#endif
}


//---------------------------------------------------------------------------
//	@function:
//		syslib::CloseLog
//
//	@doc:
//		Close the descriptor being used to write to the system logger
//
//---------------------------------------------------------------------------
void
gpos::syslib::CloseLog()
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    std::cout << "CloseLog not supported yet on WIN." << std::endl;
#else
    closelog();
#endif
}

// EOF
