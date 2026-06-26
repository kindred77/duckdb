//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/rule.hpp
//
//===----------------------------------------------------------------------===//
#pragma once
#include "duckdb/common/common.hpp"
#include "duckdb/optimizer/cascade/memo.hpp"
#include "duckdb/optimizer/cascade/physical_types.hpp"
#include "duckdb/planner/logical_operator.hpp"
namespace duckdb { namespace cascade {

//! Pattern for matching expressions in the memo during rule application
struct Pattern {
    LogicalOperatorType op_type;
    vector<Pattern> children;
    bool multi_leaf = false;
    Pattern(LogicalOperatorType t, bool ml = false) : op_type(t), multi_leaf(ml) {}
    Pattern(LogicalOperatorType t, vector<Pattern> ch, bool ml = false)
        : op_type(t), children(std::move(ch)), multi_leaf(ml) {}
};

//! Base class for transformation rules
class Rule {
public:
    enum class RuleType : uint8_t { EXPLORATION, IMPLEMENTATION };
    explicit Rule(RuleType type, Pattern pattern)
        : rule_type(type), pattern(std::move(pattern)) {}
    virtual ~Rule() = default;
    RuleType GetType() const { return rule_type; }
    const Pattern &GetPattern() const { return pattern; }
    virtual const char *GetName() const = 0;
private:
    RuleType rule_type;
    Pattern pattern;
};

//! Exploration rule: produces logical alternatives (same logical operator type space)
class ExplorationRule : public Rule {
public:
    explicit ExplorationRule(Pattern pattern) : Rule(RuleType::EXPLORATION, std::move(pattern)) {}
    virtual void Transform(const GroupExpression &input,
                           vector<pair<LogicalOperatorType, vector<GroupID>>> &results) const = 0;
};

//! Implementation rule: converts logical expressions to physical expressions
class ImplementationRule : public Rule {
public:
    explicit ImplementationRule(Pattern pattern) : Rule(RuleType::IMPLEMENTATION, std::move(pattern)) {}
    virtual void Transform(const GroupExpression &input,
                           vector<pair<PhysicalOperatorType, vector<GroupID>>> &results) const = 0;
};

}}
