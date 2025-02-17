#include "duckdb/optimizer/orca/Group.hpp"
#include "duckdb/planner/logical_operator.hpp"

namespace duckdb {

class GroupExpression;
using GroupExpressionPtr = std::shared_ptr<GroupExpression>;

using LogicalOperatorPtr = std::shared_ptr<LogicalOperator>;

class GroupExpression {
private:
    LogicalOperatorPtr operator{nullptr};
    GroupPtr group{nullptr};
public:
    GroupPtr
	Group() const
	{
		return group;
	}

    LogicalOperatorPtr
	op() const
	{
		//GPOS_ASSERT(nullptr != m_pop);
		return operator;
	}
};

}