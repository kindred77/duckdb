//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/join_implementation.hpp
//
//===----------------------------------------------------------------------===//
#pragma once
#include "duckdb/optimizer/cascade/rule.hpp"
#include "duckdb/optimizer/cascade/physical_types.hpp"
namespace duckdb { namespace cascade {

//! Implementation rule: LogicalComparisonJoin => HashJoin (physical)
//! Produces a physical hash join from a logical comparison join.
class HashJoinImplementationRule : public ImplementationRule {
public:
    HashJoinImplementationRule();
    void Transform(const GroupExpression &input,
                   vector<pair<PhysicalOperatorType, vector<GroupID>>> &results) const override;
    const char *GetName() const override;
};

}}
