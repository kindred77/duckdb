#include "duckdb/optimizer/cascade/memo.hpp"
namespace duckdb { namespace cascade {
Memo::Memo() {}

GroupID Memo::InsertExpression(LogicalOperatorType op_type, vector<GroupID> children,
                                const LogicalOperator *op_ptr) {
    GroupExpression temp(op_type, children, DConstants::INVALID_INDEX, DConstants::INVALID_INDEX);
    auto &bucket = expression_map[temp.hash];
    for (auto &[gid, eid] : bucket) {
        auto &existing = *groups[gid]->logical_expressions[eid];
        if (existing.op_type == op_type && existing.children == children) {
            return gid;
        }
    }
    auto gid = groups.size();
    auto eid = (ExprID)bucket.size();
    groups.push_back(make_uniq<Group>(gid));
    auto expr = make_uniq<GroupExpression>(op_type, children, gid, eid, op_ptr);
    groups[gid]->logical_expressions.push_back(std::move(expr));
    groups[gid]->root_op = op_type;
    bucket.emplace_back(gid, eid);
    return gid;
}

void Memo::InsertPhysical(GroupID group_id, PhysicalOperatorType phys_op_type,
                           vector<GroupID> children, double cost,
                           const LogicalOperator *source_op) {
    D_ASSERT(group_id < groups.size());
    auto pexpr = make_uniq<PhysicalExpression>(phys_op_type, std::move(children), cost, source_op);
    groups[group_id]->physical_expressions.push_back(std::move(pexpr));
}

PhysicalExpression *Memo::FinalizeBestPhysical(GroupID group_id) {
    auto &group = *groups[group_id];
    PhysicalExpression *best = nullptr;
    for (auto &pexpr : group.physical_expressions) {
        if (!best || pexpr->cost < best->cost) {
            best = pexpr.get();
        }
    }
    if (best) {
        group.best_physical = make_uniq<PhysicalExpression>(*best);
        group.best_cost = best->cost;
    }
    return group.best_physical.get();
}

Group &Memo::GetGroup(GroupID id) { D_ASSERT(id < groups.size()); return *groups[id]; }
const Group &Memo::GetGroup(GroupID id) const { D_ASSERT(id < groups.size()); return *groups[id]; }
void Memo::Dump() const {}
}}
