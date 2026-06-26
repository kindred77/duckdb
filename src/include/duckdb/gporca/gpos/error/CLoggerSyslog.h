//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CLoggerSyslog.h
//
//	@doc:
//		Implementation of logging interface over syslog
//---------------------------------------------------------------------------
#ifndef GPOS_CLoggerSyslog_H
#define GPOS_CLoggerSyslog_H

#include "gpos/error/CLogger.h"

#define GPOS_SYSLOG_ALERT(szMsg) CLoggerSyslog::Alert(GPOS_WSZ_LIT(szMsg))

namespace gpos
{
//---------------------------------------------------------------------------
//	@class:
//		CLoggerSyslog
//
//	@doc:
//		Syslog logging.
//
//---------------------------------------------------------------------------

class CLoggerSyslog : public CLogger
{
private:
	// executable name
	const CHAR *m_proc_name;

	// initialization flags
	GP_ULONG m_init_mask;

	// message priotity
	GP_ULONG m_message_priority;

	// no copy ctor
	CLoggerSyslog(const CLoggerSyslog &);

	// write string to syslog
	void Write(const WCHAR *log_entry, GP_ULONG severity);

	static CLoggerSyslog m_alert_logger;

public:
	// ctor
	CLoggerSyslog(const CHAR *proc_name, GP_ULONG init_mask,
				  GP_ULONG message_priority);

	// dtor
	virtual ~CLoggerSyslog();

	// write alert message to syslog - use ASCII characters only
	static void Alert(const WCHAR *msg);

};	// class CLoggerSyslog
}  // namespace gpos

#endif	// !GPOS_CLoggerSyslog_H

// EOF
