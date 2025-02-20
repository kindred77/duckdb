#include "duckdb/optimizer/orca/search/Group.hpp"
#include "duckdb/planner/logical_operator.hpp"

namespace duckdb {

class GroupExpression;
using GroupExpressionPtr = std::shared_ptr<GroupExpression>;

using LogicalOperatorPtr = std::shared_ptr<LogicalOperator>;

class GroupExpression {
private:
    LogicalOperatorPtr m_pop{nullptr};
    GroupPtr m_pgroup{nullptr};
public:
    GroupPtr
	Pgroup() const
	{
		return m_pgroup;
	}

    LogicalOperatorPtr
	op() const
	{
		//GPOS_ASSERT(nullptr != m_pop);
		return m_pop;
	}
};

}