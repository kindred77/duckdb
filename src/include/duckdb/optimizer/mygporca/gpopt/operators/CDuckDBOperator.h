#ifndef GPOPT_CDuckDBOperator_H
#define GPOPT_CDuckDBOperator_H

#include "gpos/base.h"
#include "gpos/common/CRefCount.h"
#include "gpos/common/DbgPrintMixin.h"

#include "duckdb/planner/logical_operator.hpp"
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

	unique_ptr<LogicalOperator> m_logical_opt;

	unique_ptr<PhysicalOperator> m_physical_opt;

public:
	enum EDOperatorId {
		//logical
		EDopLogicalGet,
		EDopLogicalProject,
		EDopLogicalSelect,
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

	// dtor
	virtual ~CDuckDBOperator()
	{
	}

	// is operator logical?
	virtual BOOL
	FLogical() const
	{
		return false;
	}

	// is operator physical?
	virtual BOOL
	FPhysical() const
	{
		return false;
	}

	// is operator scalar?
	virtual BOOL
	FScalar() const
	{
		return false;
	}
};

}
#endif // GPOPT_CDuckDBOperator_H

