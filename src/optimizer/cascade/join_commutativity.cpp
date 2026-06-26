#include "duckdb/optimizer/cascade/join_commutativity.hpp"
namespace duckdb { namespace cascade {

JoinCommutativityRule::JoinCommutativityRule()
    : ExplorationRule(Pattern(LogicalOperatorType::LOGICAL_COMPARISON_JOIN)) {}

void JoinCommutativityRule::Transform(
    const GroupExpression &input,
    vector<pair<LogicalOperatorType, vector<GroupID>>> &results) const {

    // Only swap if the join has exactly 2 children
    if (input.children.size() != 2) return;

    // Produce (B JOIN A) - same operator type, swapped children
    vector<GroupID> swapped = {input.children[1], input.children[0]};
    results.emplace_back(input.op_type, std::move(swapped));
}

const char *JoinCommutativityRule::GetName() const {
    return "Join Commutativity";
}

}}
