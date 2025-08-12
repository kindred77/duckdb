//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		exception.h
//
//	@doc:
//		Definition of GPOPT-specific exception types
//---------------------------------------------------------------------------
#ifndef GPOPT_exception_H
#define GPOPT_exception_H

#include "gpos/memory/CMemoryPool.h"
#include "gpos/types.h"

namespace gpopt
{
// major exception types - reserve range 1000-2000
enum ExMajor
{
	ExmaGPOPT = 1000,

	ExmaSentinel
};

// minor exception types
enum ExMinor
{
	ExmiNoPlanFound,
	ExmiInvalidPlanAlternative,
	ExmiUnsupportedOp,
	ExmiUnexpectedOp,
	ExmiUnsupportedPred,
	ExmiUnsupportedCompositePartKey,
	ExmiUnsupportedNonDeterministicUpdate,
	ExmiUnsatisfiedRequiredProperties,
	ExmiEvalUnsupportedScalarExpr,
	ExmiCTEProducerConsumerMisAligned,
	ExmiNoStats,

	ExmiSentinel
};

// message initialization for GPOS exceptions
gpos::GPOS_RESULT EresExceptionInit(gpos::CMemoryPool *mp);

}  // namespace gpopt

namespace gpdxl
{
// major exception types - reserve range 200-1000
enum ExMajor
{
	ExmaDXL = 200,
	ExmaMD = 300,
	ExmaComm = 400,
	ExmaGPDB = 500,
	ExmaConstExprEval = 600,

	ExmaSentinel
};

// minor exception types
enum ExMinor
{
	// DXL-parsing related errors
	ExmiDXLUnexpectedTag,
	ExmiDXLMissingAttribute,
	ExmiDXLInvalidAttributeValue,
	ExmiDXLUnrecognizedOperator,
	ExmiDXLUnrecognizedType,
	ExmiDXLUnrecognizedCompOperator,
	ExmiDXLValidationError,
	ExmiDXLXercesParseError,
	ExmiDXLIncorrectNumberOfChildren,
	ExmiPlStmt2DXLConversion,
	ExmiDXL2PlStmtConversion,
	ExmiDXL2PlStmtExternalScanError,
	ExmiDXL2PlStmtMissingPlanForSubPlanTranslation,
	ExmiQuery2DXLAttributeNotFound,
	ExmiQuery2DXLUnsupportedFeature,
	ExmiQuery2DXLDuplicateRTE,
	ExmiQuery2DXLMissingValue,
	ExmiQuery2DXLNotNullViolation,
	ExmiQuery2DXLError,
	ExmiExpr2DXLUnsupportedFeature,
	ExmiExpr2DXLAttributeNotFound,
	ExmiDXL2PlStmtAttributeNotFound,
	ExmiDXL2ExprAttributeNotFound,

	// MD related errors
	ExmiMDCacheEntryDuplicate,
	ExmiMDCacheEntryNotFound,
	ExmiMDObjUnsupported,

	// communication related errors
	ExmiCommPropagateError,
	ExmiCommUnexpectedMessage,

	// GPDB-related exceptions
	ExmiGPDBError,

	// exceptions related to constant expression evaluation
	ExmiConstExprEvalNonConst,

	// ORCA Exceptions that need to be reported as ERROR to GPDB
	ExmiOptimizerError,
	ExmiNoAvailableMemory,
	ExmiInvalidComparisonTypeCode,

	ExmiDXLSentinel
};

// message initialization for GPOS exceptions
gpos::GPOS_RESULT EresExceptionInit(gpos::CMemoryPool *mp);

}  // namespace gpdxl

#endif	// !GPOPT_exception_H


// EOF
