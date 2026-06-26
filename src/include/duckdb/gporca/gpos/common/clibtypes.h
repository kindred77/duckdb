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

#include <gpos/dlfcn.h>
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
typedef struct timeval TIMEVAL;

// hold minimal information about the local time zone
typedef struct timezone TIMEZONE;

// represents an elapsed time
typedef struct timespec TIMESPEC;

// store system time values
typedef time_t TIME_T;

// containing a calendar date and time broken down into its components.
typedef struct tm TIME;

// store information of a calling process
typedef Dl_info DL_INFO;
}  // namespace gpos

#endif	// !GPOS_clibtypes_H

// EOF
