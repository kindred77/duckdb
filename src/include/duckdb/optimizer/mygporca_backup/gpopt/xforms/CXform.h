//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXform.h
//
//	@doc:
//		Base class for all transformations: substitution, exploration,
//		and implementation
//---------------------------------------------------------------------------
#ifndef DUCKDB_CXFORM_H
#define DUCKDB_CXFORM_H

#include "gpos/base.h"
#include "gpos/common/CEnumSet.h"
#include "gpos/common/CEnumSetIter.h"
#include "gpos/common/CRefCount.h"

#include "gpopt/operators/CExpression.h"
#include "gpopt/xforms/CXformResult.h"
#include "gpopt/xforms/CXformContext.h"

// Macro for enabling and disabling xforms
#define GPOPT_DISABLE_XFORM_TF(x) EopttraceDisableXformBase + x
#define GPOPT_ENABLE_XFORM(x) GPOS_UNSET_TRACE(GPOPT_DISABLE_XFORM_TF(x))
#define GPOPT_DISABLE_XFORM(x) GPOS_SET_TRACE(GPOPT_DISABLE_XFORM_TF(x))
#define GPOPT_FENABLED_XFORM(x) !GPOS_FTRACE(GPOPT_DISABLE_XFORM_TF(x))
#define GPOPT_FDISABLED_XFORM(x) GPOS_FTRACE(GPOPT_DISABLE_XFORM_TF(x))


namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CXform
//
//	@doc:
//		base class for all transformations
//
//---------------------------------------------------------------------------
class CXform : public CRefCount, public DbgPrintMixin<CXform>
{
private:
	// pattern
	CExpression *m_pexpr;

	// private copy ctor
	CXform(CXform &);

public:
	// identification
	//
	// IMPORTANT: when adding new Xform Ids, please add them near
	// the end of the enum (before ExfInvalid). Xform Ids are sometimes
	// referenced using their location in the array (e.g. when disabling
	// xforms using traceflags), so shifting these ids may result in
	// accidentally disabling the wrong xform

	enum EXformId
	{
		ExfGet2TableScan = 0,

		ExfInvalid,
		ExfSentinel = ExfInvalid
	};

	// promise levels;
	// used for prioritizing xforms as well as bypassing inapplicable xforms
	enum EXformPromise
	{
		ExfpNone,	 // xform must not be used as it fails a precondition
		ExfpLow,	 // xform has low priority
		ExfpMedium,	 // xform has medium priority
		ExfpHigh	 // xform has high priority
	};

	// ctor
	explicit CXform(CExpression *pexpr);

	// dtor
	virtual ~CXform();

	// ident accessors
	virtual EXformId Exfid() const = 0;

	// return a string for xform name
	virtual const CHAR *SzId() const = 0;

	// the following functions check xform type

	// is xform substitution?
	virtual BOOL
	FSubstitution() const
	{
		return false;
	}

	// is xform exploration?
	virtual BOOL
	FExploration() const
	{
		return false;
	}

	// is xform implementation?
	virtual BOOL
	FImplementation() const
	{
		return false;
	}

	// actual transformation
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const = 0;

	// accessor
	CExpression *
	PexprPattern() const
	{
		return m_pexpr;
	}

	// check compatibility with another xform
	virtual BOOL FCompatible(CXform::EXformId)
	{
		return true;
	}

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

	// equality function over xform ids
	static BOOL FEqualIds(const CHAR *szIdOne, const CHAR *szIdTwo);

	// return true if xform should be applied only once.
	// for expression of type CPatternTree, in deep trees, the number
	// of expressions generated for group expression can be significantly
	// large causing the Xform to be applied many times. This can lead to
	// significantly long planning time, so such Xform should only be applied once
	virtual BOOL IsApplyOnce();

};	// class CXform

// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CXform &xform)
{
	return xform.OsPrint(os);
}

// shorthands for enum sets and iterators of xform ids
typedef CEnumSet<CXform::EXformId, CXform::ExfSentinel> CXformSet;
typedef CEnumSetIter<CXform::EXformId, CXform::ExfSentinel> CXformSetIter;
}  // namespace gpopt


#endif	// !DUCKDB_CXFORM_H

// EOF
