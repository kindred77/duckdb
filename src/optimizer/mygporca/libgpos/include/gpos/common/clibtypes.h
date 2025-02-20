//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		types.h
//
//	@doc:
//		clib definitions for GPOS;
//---------------------------------------------------------------------------
#ifndef GPOS_clibtypes_H
#define GPOS_clibtypes_H

#include <dlfcn.h>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//#include <windows.h>
#else
#include <sys/resource.h>
#endif
#include <sys/time.h>
#include <time.h>

#include "gpos/types.h"

namespace gpos
{
// container for user and system time
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
struct RUSAGE {
  struct timeval ru_utime;
  struct timeval ru_stime;
};
#else
using RUSAGE = struct rusage;
#endif

// represent an elapsed time
using TIMEVAL = struct timeval;

// hold minimal information about the local time zone
using TIMEZONE = struct timezone;

// represents an elapsed time
using TIMESPEC = struct timespec;

// store system time values
using TIME_T = time_t;

// containing a calendar date and time broken down into its components.
using TIME = struct tm;

// store information of a calling process
using DL_INFO = Dl_info;
}  // namespace gpos

#endif	// !GPOS_clibtypes_H

// EOF
