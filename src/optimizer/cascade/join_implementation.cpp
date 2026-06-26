#include "duckdb/optimizer/cascade/join_implementation.hpp"
namespace duckdb { namespace cascade {

HashJoinImplementationRule::HashJoinImplementationRule()
    : ImplementationRule(Pattern(LogicalOperatorType::LOGICAL_COMPARISON_JOIN)) {}

void HashJoinImplementationRule::Transform(
    const GroupExpression &input,
    vector<pair<PhysicalOperatorType, vector<GroupID>>> &results) const {

    // Only implement joins with exactly 2 children
    if (input.children.size() != 2) return;

    // Produce HASH_JOIN with the same children
    results.emplace_back(PhysicalOperatorType::HASH_JOIN, input.children);
}

const char *HashJoinImplementationRule::GetName() const {
    return "Hash Join Implementation";
}

}}
