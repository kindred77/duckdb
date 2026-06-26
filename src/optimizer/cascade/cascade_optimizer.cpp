#include "duckdb/optimizer/cascade/cascade_optimizer.hpp"
#include "duckdb/optimizer/optimizer.hpp"
#include "duckdb/optimizer/cascade/join_commutativity.hpp"
#include "duckdb/optimizer/cascade/join_implementation.hpp"
#include <cstdio>
namespace duckdb { namespace cascade {

CascadeOptimizer::CascadeOptimizer(Optimizer &optimizer_p) : optimizer(optimizer_p) {
    search = make_uniq<SearchEngine>(memo);
    RegisterBuiltInRules();
}

GroupID CascadeOptimizer::BuildMemo(LogicalOperator &op) {
    vector<GroupID> child_groups;
    for (auto &child : op.children) {
        child_groups.push_back(BuildMemo(*child));
    }
    return memo.InsertExpression(op.type, child_groups, &op);
}

unique_ptr<LogicalOperator> CascadeOptimizer::ExtractPlan(GroupID group_id) {
    auto &group = memo.GetGroup(group_id);
    PhysicalExpression *best = group.best_physical
        ? group.best_physical.get()
        : (!group.physical_expressions.empty() ? group.physical_expressions[0].get() : nullptr);
    if (!best) {
        if (!group.logical_expressions.empty() && group.logical_expressions[0]->op) {
            auto result = group.logical_expressions[0]->op->Copy(optimizer.context);
            for (auto child_id : group.logical_expressions[0]->children) {
                auto child = ExtractPlan(child_id);
                if (child) result->AddChild(std::move(child));
            }
            return result;
        }
        return nullptr;
    }
    auto &src_expr = *group.logical_expressions[0];
    if (src_expr.op) {
        auto result = src_expr.op->Copy(optimizer.context);
        for (auto child_id : best->children) {
            auto child = ExtractPlan(child_id);
            if (child) result->AddChild(std::move(child));
        }
        return result;
    }
    return nullptr;
}

void CascadeOptimizer::RegisterBuiltInRules() {
    search->AddExplorationRule(make_uniq<JoinCommutativityRule>());
    search->AddImplementationRule(make_uniq<HashJoinImplementationRule>());
}

unique_ptr<LogicalOperator> CascadeOptimizer::Optimize(unique_ptr<LogicalOperator> plan) {
    if (!plan) return plan;
    auto root_group = BuildMemo(*plan);

    fprintf(stderr, "[Cascade] Starting search: %zu groups, %zu explore rules, %zu impl rules\n",
            memo.GroupCount(), search->ExplorationRuleCount(), search->ImplementationRuleCount());

    auto result = search->Optimize(root_group);

    auto &root = memo.GetGroup(root_group);
    fprintf(stderr, "[Cascade] Search complete: %zu groups, best_cost=%.2f, "
            "%zu physical_expressions at root\n",
            memo.GroupCount(), result.cost,
            root.physical_expressions.size());

    return plan;
}

}}
