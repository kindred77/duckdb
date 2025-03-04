#include "duckdb/optimizer/orca/search/Group.hpp"
#include "duckdb/optimizer/orca/search/Expression.hpp"
#include "duckdb/optimizer/orca/search/GroupExpression.hpp"

namespace duckdb {

class Memo;
using MemoPtr = std::shared_ptr<Memo>;

class Memo {
private:
    GroupPtr m_pgroupRoot;
    size_t m_ulpGrps;

public:
    Memo();

    // return root group
	GroupPtr
	PgroupRoot() const
	{
		return m_pgroupRoot;
	}

    void SetRoot(GroupPtr group_ptr);

    GroupPtr PgroupInsert(GroupPtr group_target, ExpressionPtr expr_origin,
					GroupExpressionPtr group_expr);
    
    size_t
	UlpGroups() const
	{
		return m_ulpGrps;
	}

    // extract a plan that delivers the given required properties
	ExpressionPtr PexprExtractPlan(GroupPtr pgroupRoot,
								  ReqdPropPlanPtr prppInput,
								  uint32_t ulSearchStages);
	
	// merge duplicate groups
	void GroupMerge();

	// reset states of all memo groups
	void ResetGroupStates();

	// reset statistics of memo groups
	void ResetStats();
};

}