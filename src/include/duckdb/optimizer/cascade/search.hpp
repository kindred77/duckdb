//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/search.hpp
//
//===----------------------------------------------------------------------===//
#pragma once
#include "duckdb/common/common.hpp"
#include "duckdb/optimizer/cascade/memo.hpp"
#include "duckdb/optimizer/cascade/rule.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include <functional>
namespace duckdb { namespace cascade {

enum class TaskType : uint8_t {
    EXPLORE_GROUP,
    EXPLORE_EXPRESSION,
    OPTIMIZE_GROUP,
    OPTIMIZE_EXPRESSION,
    APPLY_RULE,
};

struct Task {
    TaskType type;
    GroupID group_id = DConstants::INVALID_INDEX;
    ExprID expr_id = DConstants::INVALID_INDEX;
    idx_t rule_idx = DConstants::INVALID_INDEX;
    Task(TaskType t) : type(t) {}
    Task(TaskType t, GroupID gid) : type(t), group_id(gid) {}
    Task(TaskType t, GroupID gid, ExprID eid) : type(t), group_id(gid), expr_id(eid) {}
    Task(TaskType t, GroupID gid, ExprID eid, idx_t ridx)
        : type(t), group_id(gid), expr_id(eid), rule_idx(ridx) {}
};

struct OptimizeResult {
    GroupID root_group = DConstants::INVALID_INDEX;
    double cost = 0.0;
};

class SearchEngine {
public:
    explicit SearchEngine(Memo &memo);

    void AddExplorationRule(unique_ptr<ExplorationRule> rule);
    void AddImplementationRule(unique_ptr<ImplementationRule> rule);
    OptimizeResult Optimize(GroupID root_group);

    // Accessors for debug logging
    idx_t ExplorationRuleCount() const { return exploration_rules.size(); }
    idx_t ImplementationRuleCount() const { return implementation_rules.size(); }

private:
    void PushTask(Task task);
    void Run();

    void ExploreGroup(GroupID group_id);
    void ExploreExpression(GroupID group_id, ExprID expr_id);
    void ApplyExplorationRule(GroupID group_id, ExprID expr_id, idx_t rule_idx);

    void OptimizeGroup(GroupID group_id);
    void OptimizeExpression(GroupID group_id, ExprID expr_id);

    void GetApplicableExplorationRules(const GroupExpression &expr, vector<idx_t> &rule_indices) const;
    bool MatchesPattern(const GroupExpression &expr, const Pattern &pattern) const;

    Memo &memo;
    vector<unique_ptr<ExplorationRule>> exploration_rules;
    vector<unique_ptr<ImplementationRule>> implementation_rules;
    vector<Task> task_stack;
    unordered_set<hash_t> explored_set;
    unordered_set<GroupID> optimized_groups;
};

}}
