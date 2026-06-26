//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/join_commutativity.hpp
//
//===----------------------------------------------------------------------===//
#pragma once
#include "duckdb/optimizer/cascade/rule.hpp"
namespace duckdb { namespace cascade {

//! Exploration rule: (A JOIN B) => (B JOIN A)
//! Swaps the children of a join to produce an equivalent logical alternative.
class JoinCommutativityRule : public ExplorationRule {
public:
    JoinCommutativityRule();
    void Transform(const GroupExpression &input,
                   vector<pair<LogicalOperatorType, vector<GroupID>>> &results) const override;
    const char *GetName() const override;
};

}}
