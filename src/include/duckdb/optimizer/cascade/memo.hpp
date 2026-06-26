//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/memo.hpp
//
//===----------------------------------------------------------------------===//
#pragma once

#include "duckdb/common/common.hpp"
#include "duckdb/common/unordered_map.hpp"
#include "duckdb/common/unordered_set.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/optimizer/cascade/physical_types.hpp"

namespace duckdb {
namespace cascade {

using GroupID = idx_t;
using ExprID = idx_t;

//! A GroupExpression represents a logical operator in the Memo.
struct GroupExpression {
    LogicalOperatorType op_type;
    vector<GroupID> children;
    GroupID group_id;
    ExprID expr_id;
    bool explored = false;
    hash_t hash;
    const LogicalOperator *op;

    GroupExpression(LogicalOperatorType op, vector<GroupID> ch, GroupID gid, ExprID eid,
                    const LogicalOperator *op_ptr = nullptr)
        : op_type(op), children(std::move(ch)), group_id(gid), expr_id(eid), op(op_ptr) {
        ComputeHash();
    }

    bool operator==(const GroupExpression &o) const {
        if (op_type != o.op_type || children.size() != o.children.size()) return false;
        for (size_t i = 0; i < children.size(); i++)
            if (children[i] != o.children[i]) return false;
        return true;
    }

    void ComputeHash() {
        hash = Hash(op_type);
        for (auto &c : children) hash = CombineHash(hash, Hash(c));
    }
};

struct GroupExpressionHash {
    hash_t operator()(const GroupExpression &g) const { return g.hash; }
};

//! A PhysicalExpression represents a concrete physical implementation
struct PhysicalExpression {
    PhysicalOperatorType phys_op_type;
    vector<GroupID> children;
    double cost;
    const LogicalOperator *source_op;

    PhysicalExpression(PhysicalOperatorType ptype, vector<GroupID> ch, double c = 0.0,
                       const LogicalOperator *src = nullptr)
        : phys_op_type(ptype), children(std::move(ch)), cost(c), source_op(src) {}
};

//! A Group contains all equivalent logical and physical expressions
struct Group {
    GroupID group_id;
    vector<unique_ptr<GroupExpression>> logical_expressions;
    vector<unique_ptr<PhysicalExpression>> physical_expressions;
    unique_ptr<PhysicalExpression> best_physical;
    LogicalOperatorType root_op;
    double best_cost = -1.0;

    explicit Group(GroupID id) : group_id(id), root_op(LogicalOperatorType::LOGICAL_INVALID) {}
};

class Memo {
public:
    Memo();

    GroupID InsertExpression(LogicalOperatorType op_type, vector<GroupID> children,
                             const LogicalOperator *op_ptr = nullptr);

    void InsertPhysical(GroupID group_id, PhysicalOperatorType phys_op_type,
                        vector<GroupID> children, double cost,
                        const LogicalOperator *source_op = nullptr);

    Group &GetGroup(GroupID id);
    const Group &GetGroup(GroupID id) const;

    idx_t GroupCount() const { return groups.size(); }

    PhysicalExpression *FinalizeBestPhysical(GroupID group_id);

    void Dump() const;

private:
    vector<unique_ptr<Group>> groups;
    unordered_map<hash_t, vector<pair<GroupID, ExprID>>> expression_map;
};

} // namespace cascade
} // namespace duckdb
