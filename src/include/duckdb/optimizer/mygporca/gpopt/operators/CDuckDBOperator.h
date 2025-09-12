#ifndef GPOPT_CDuckDBOperator_H
#define GPOPT_CDuckDBOperator_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"
#include "gpos/common/DbgPrintMixin.h"

#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/expression.hpp"
#include "duckdb/execution/physical_operator.hpp"
#include "duckdb/common/unique_ptr.hpp"

namespace gpopt
{

using namespace gpos;
using namespace duckdb;

class CDuckDBOperator : public CRefCount, public gpos::DbgPrintMixin<CDuckDBOperator>
{
private:
	// private copy ctor
	CDuckDBOperator(CDuckDBOperator &);
protected:
	// memory pool
	CMemoryPool *m_mp;

	LogicalOperator *m_logical_opt;
	BOOL fLogical = false;

	PhysicalOperator *m_physical_opt;
	BOOL fPhysical = false;

	Expression *m_expression;
	BOOL fFScalar =  false;

public:
	enum EDOperatorId {
		//logical
		EDopLogicalGet,
		EDopLogicalProject,
		EDopLogicalFilter,
		EDopLogicalJoin,
		EDopLogicalCTEProducer,
		EDopLogicalCTEConsumer,
		EDopLogicalCTEAnchor,
		EDopLogicalLimit,
		EDopLogicalGrpBy,
		EDopLogicalGrpCl,
		EDopLogicalGrpCols,
		EDopLogicalConstTable,
		EDopLogicalTVF,
		EDopLogicalSetOp,
		EDopLogicalWindow,
		EDopLogicalCTAS,

		//scalar
		EDopScalarCmp,
		EDopScalarDistinct,
		EDopScalarIdent,
		EDopScalarProjectList,
		EDopScalarProjectElem,
		EDopScalarConstTuple,
		EDopScalarFilter,
		EDopScalarOneTimeFilter,
		EDopScalarJoinFilter,
		EDopScalarRecheckCondFilter,
		EDopScalarHashExprList,
		EDopScalarHashExpr,
		EDopScalarHashCondList,
		EDopScalarArray,
		EDopScalarArrayRef,
		EDopScalarArrayRefIndexList,

		EDopScalarAssertConstraintList,
		EDopScalarAssertConstraint,

		EDopScalarMergeCondList,

		EDopScalarIndexCondList,

		EDopScalarSortColList,
		EDopScalarSortCol,

		EDopScalarQueryOutput,

		EDopScalarOpExpr,
		EDopScalarBoolExpr,
		EDopScalarCaseTest,
		EDopScalarCoalesce,
		EDopScalarConstValue,
		EDopScalarIfStmt,
		EDopScalarSwitch,
		EDopScalarSwitchCase,
		EDopScalarLimitCount,
		EDopScalarLimitOffset,
		EDopScalarFuncExpr,
		EDopScalarMinMax,
		EDopScalarWindowRef,
		EDopScalarNullTest,
		EDopScalarNullIf,
		EDopScalarCast,
		EDopScalarCoerceToDomain,
		EDopScalarCoerceViaIO,
		EDopScalarArrayCoerceExpr,
		EDopScalarAggref,
		EDopScalarArrayComp,
		EDopScalarBooleanTest,
		EDopScalarSubPlan,
		EDopScalarWindowFrameEdge,

		EDopScalarSubquery,
		EDopScalarSubqueryAny,
		EDopScalarSubqueryAll,
		EDopScalarSubqueryExists,
		EDopScalarSubqueryNotExists,
		EDopScalarBitmapBoolOp,

		EDopScalarDMLAction,
		EDopScalarOpList,
		EDopScalarPartOid,
		EDopScalarPartDefault,
		EDopScalarPartBound,
		EDopScalarPartBoundInclusion,
		EDopScalarPartBoundOpen,
		EDopScalarPartListValues,
		EDopScalarPartListNullTest,
		EDopScalarValuesList,
		EDopScalarSortGroupClause,

		//physical
		EDopPhysicalResult,
		EDopPhysicalValuesScan,
		EDopPhysicalProjection,
		EDopPhysicalTableScan,
		EDopPhysicalBitmapTableScan,
		EDopPhysicalDynamicBitmapTableScan,
		EDopPhysicalExternalScan,
		EDopPhysicalMultiExternalScan,
		EDopPhysicalIndexScan,
		EDopPhysicalIndexOnlyScan,
		EDopScalarBitmapIndexProbe,
		EDopPhysicalSubqueryScan,
		EDopPhysicalConstTable,
		EDopPhysicalNLJoin,
		EDopPhysicalHashJoin,
		EDopPhysicalMergeJoin,
		EDopPhysicalMotionGather,
		EDopPhysicalMotionBroadcast,
		EDopPhysicalMotionRedistribute,
		EDopPhysicalMotionRoutedDistribute,
		EDopPhysicalMotionRandom,
		EDopPhysicalLimit,
		EDopPhysicalAgg,
		EDopPhysicalSort,
		EDopPhysicalAppend,
		EDopPhysicalMaterialize,
		EDopPhysicalSequence,
		EDopPhysicalDynamicTableScan,
		EDopPhysicalDynamicIndexScan,
		EDopPhysicalPartitionSelector,
		EDopPhysicalTVF,
		EDopPhysicalWindow,

		EDopPhysicalCTEProducer,
		EDopPhysicalCTEConsumer,

		EDopPhysicalDML,
		EDopPhysicalSplit,
		EDopPhysicalRowTrigger,

		EDopPhysicalAssert,

		//end
		EDopSentinel
	};

	// ctor
	explicit CDuckDBOperator(CMemoryPool *mp);

	explicit CDuckDBOperator(CMemoryPool *mp, LogicalOperator *logical_opt)
	    : m_logical_opt(logical_opt), fLogical(true)
	{

	}

	explicit CDuckDBOperator(CMemoryPool *mp, PhysicalOperator *physical_opt)
	    : m_physical_opt(physical_opt), fPhysical(true)
	{

	}

	explicit CDuckDBOperator(CMemoryPool *mp, Expression *expression)
	    : m_expression(expression), fPhysical(true)
	{

	}

	// dtor
	virtual ~CDuckDBOperator()
	{
	}

	// is operator logical?
	virtual BOOL
	FLogical() const
	{
		return fLogical;
	}

	// is operator physical?
	virtual BOOL
	FPhysical() const
	{
		return fPhysical;
	}

	// is operator scalar?
	virtual BOOL
	FScalar() const
	{
		return fFScalar;
	}
};

}
#endif // GPOPT_CDuckDBOperator_H

