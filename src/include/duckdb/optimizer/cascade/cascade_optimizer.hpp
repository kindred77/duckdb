//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/cascade_optimizer.hpp
//
//===----------------------------------------------------------------------===//
#pragma once
#include "duckdb/common/common.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/optimizer/cascade/memo.hpp"
#include "duckdb/optimizer/cascade/search.hpp"
#include "duckdb/optimizer/cascade/rule.hpp"
namespace duckdb {
class Optimizer;
class LogicalOperatorVisitor;
namespace cascade {

class CascadeOptimizer {
public:
    explicit CascadeOptimizer(Optimizer &optimizer);
    unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> plan);
    void RegisterBuiltInRules();
private:
    GroupID BuildMemo(LogicalOperator &op);
    unique_ptr<LogicalOperator> ExtractPlan(GroupID group_id);
    Optimizer &optimizer;
    Memo memo;
    unique_ptr<SearchEngine> search;
};

}}