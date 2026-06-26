#include "duckdb/optimizer/cascade/search.hpp"
namespace duckdb { namespace cascade {

SearchEngine::SearchEngine(Memo &memo_p) : memo(memo_p) {}

void SearchEngine::AddExplorationRule(unique_ptr<ExplorationRule> rule) {
    exploration_rules.push_back(std::move(rule));
}

void SearchEngine::AddImplementationRule(unique_ptr<ImplementationRule> rule) {
    implementation_rules.push_back(std::move(rule));
}

void SearchEngine::PushTask(Task task) { task_stack.push_back(task); }

bool SearchEngine::MatchesPattern(const GroupExpression &expr, const Pattern &pattern) const {
    if (pattern.op_type == LogicalOperatorType::LOGICAL_INVALID) return true;
    return expr.op_type == pattern.op_type;
}

void SearchEngine::GetApplicableExplorationRules(const GroupExpression &expr, vector<idx_t> &rule_indices) const {
    for (idx_t i = 0; i < exploration_rules.size(); i++) {
        if (MatchesPattern(expr, exploration_rules[i]->GetPattern())) {
            rule_indices.push_back(i);
        }
    }
}

//===----------------------------------------------------------------------===//
// Explore phase
//===----------------------------------------------------------------------===//

void SearchEngine::ExploreGroup(GroupID group_id) {
    auto &group = memo.GetGroup(group_id);
    for (idx_t i = 0; i < group.logical_expressions.size(); i++) {
        PushTask(Task(TaskType::EXPLORE_EXPRESSION, group_id, i));
    }
}

void SearchEngine::ExploreExpression(GroupID group_id, ExprID expr_id) {
    auto &expr = *memo.GetGroup(group_id).logical_expressions[expr_id];
    for (auto child_id : expr.children) {
        PushTask(Task(TaskType::EXPLORE_GROUP, child_id));
    }
    if (expr.explored) return;
    expr.explored = true;

    vector<idx_t> applicable;
    GetApplicableExplorationRules(expr, applicable);
    for (auto r : applicable) {
        PushTask(Task(TaskType::APPLY_RULE, group_id, expr_id, r));
    }
}

void SearchEngine::ApplyExplorationRule(GroupID group_id, ExprID expr_id, idx_t rule_idx) {
    auto &expr = *memo.GetGroup(group_id).logical_expressions[expr_id];
    auto &rule = *exploration_rules[rule_idx];
    vector<pair<LogicalOperatorType, vector<GroupID>>> results;
    rule.Transform(expr, results);
    for (auto &[op_type, children] : results) {
        memo.InsertExpression(op_type, children);
    }
}

//===----------------------------------------------------------------------===//
// Optimize phase
//===----------------------------------------------------------------------===//

void SearchEngine::OptimizeGroup(GroupID group_id) {
    if (optimized_groups.find(group_id) != optimized_groups.end()) return;
    auto &group = memo.GetGroup(group_id);
    for (idx_t i = 0; i < group.logical_expressions.size(); i++) {
        PushTask(Task(TaskType::OPTIMIZE_EXPRESSION, group_id, i));
    }
}

void SearchEngine::OptimizeExpression(GroupID group_id, ExprID expr_id) {
    auto &expr = *memo.GetGroup(group_id).logical_expressions[expr_id];
    for (auto child_id : expr.children) {
        PushTask(Task(TaskType::OPTIMIZE_GROUP, child_id));
    }

    // Apply implementation rules to this expression
    for (auto &impl_rule : implementation_rules) {
        if (!MatchesPattern(expr, impl_rule->GetPattern())) continue;
        vector<pair<PhysicalOperatorType, vector<GroupID>>> results;
        impl_rule->Transform(expr, results);
        for (auto &[ptype, children] : results) {
            double total_cost = 0.0;
            for (auto child_id : children) {
                auto &child_group = memo.GetGroup(child_id);
                total_cost += child_group.best_cost > 0 ? child_group.best_cost : 0.1;
            }
            double op_cost = expr.op && expr.op->has_estimated_cardinality
                             ? static_cast<double>(expr.op->estimated_cardinality) * 0.01
                             : 0.1;
            if (ptype == PhysicalOperatorType::HASH_JOIN && !children.empty()) {
                auto &build_group = memo.GetGroup(children[0]);
                double build_rows = build_group.best_cost > 0 ? build_group.best_cost / 0.1 : 1000;
                op_cost += build_rows * 0.05;
            }
            total_cost += op_cost;
            memo.InsertPhysical(group_id, ptype, children, total_cost);
        }
    }

    memo.FinalizeBestPhysical(group_id);
    optimized_groups.insert(group_id);
}

//===----------------------------------------------------------------------===//
// Main execution loop
//===----------------------------------------------------------------------===//

void SearchEngine::Run() {
    while (!task_stack.empty()) {
        auto task = task_stack.back();
        task_stack.pop_back();
        switch (task.type) {
        case TaskType::EXPLORE_GROUP:
            ExploreGroup(task.group_id);
            break;
        case TaskType::EXPLORE_EXPRESSION:
            ExploreExpression(task.group_id, task.expr_id);
            break;
        case TaskType::OPTIMIZE_GROUP:
            OptimizeGroup(task.group_id);
            break;
        case TaskType::OPTIMIZE_EXPRESSION:
            OptimizeExpression(task.group_id, task.expr_id);
            break;
        case TaskType::APPLY_RULE:
            ApplyExplorationRule(task.group_id, task.expr_id, task.rule_idx);
            break;
        }
    }
}

OptimizeResult SearchEngine::Optimize(GroupID root_group) {
    PushTask(Task(TaskType::EXPLORE_GROUP, root_group));
    Run();

    PushTask(Task(TaskType::OPTIMIZE_GROUP, root_group));
    Run();

    OptimizeResult result;
    result.root_group = root_group;
    auto &root = memo.GetGroup(root_group);
    result.cost = root.best_cost > 0 ? root.best_cost : 0.0;
    return result;
}

}}
