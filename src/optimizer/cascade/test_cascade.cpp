//===----------------------------------------------------------------------===//
//                         DuckDB
//
// test/cascade/test_cascade.cpp
//
// Direct unit test of Cascade CBO core components (Memo, SearchEngine, Rules)
// without going through the full SQL pipeline.
//===----------------------------------------------------------------------===//
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>

#include "duckdb/common/common.hpp"
#include "duckdb/common/enums/logical_operator_type.hpp"
#include "duckdb/optimizer/cascade/memo.hpp"
#include "duckdb/optimizer/cascade/physical_types.hpp"
#include "duckdb/optimizer/cascade/rule.hpp"
#include "duckdb/optimizer/cascade/search.hpp"
#include "duckdb/optimizer/cascade/join_commutativity.hpp"
#include "duckdb/optimizer/cascade/join_implementation.hpp"

using namespace duckdb;
using namespace duckdb::cascade;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { fprintf(stderr, "  TEST: %s ... ", name);
#define PASS() do { fprintf(stderr, "PASS\n"); tests_passed++; } while(0)
#define FAIL(msg) do { fprintf(stderr, "FAIL: %s\n", msg); tests_failed++; } while(0)
#define CHECK(cond, msg) do { if (!(cond)) { FAIL(msg); return; } } while(0)

//===----------------------------------------------------------------------===//
// Test 1: Memo basic insertion and deduplication
//===----------------------------------------------------------------------===//
void TestMemoInsert() {
    Memo memo;

    // Insert leaf expressions (no children)
    GroupID g1 = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    GroupID g2 = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    CHECK(g1 != g2, "Two leaf GETs should be in different groups");

    // Insert same expression again ? should return same group
    GroupID g1_dup = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    CHECK(g1_dup == g1, "Duplicate leaf GET should return same group");

    // Insert join of g1 and g2
    GroupID j1 = memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {g1, g2});
    CHECK(j1 < memo.GroupCount(), "Join group should be created");

    // Insert same join again ? should return same group
    GroupID j1_dup = memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {g1, g2});
    CHECK(j1_dup == j1, "Duplicate join should return same group");

    // Insert swapped join ? should be different group (different children order)
    GroupID j2 = memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {g2, g1});
    CHECK(j2 != j1, "Swapped join should be a different group");

    CHECK(memo.GroupCount() == 4, "Should have 4 groups: 2 leaf + join + swapped join");
}

//===----------------------------------------------------------------------===//
// Test 2: Group expression equality and hashing
//===----------------------------------------------------------------------===//
void TestGroupExpressionEq() {
    GroupExpression a(LogicalOperatorType::LOGICAL_GET, {}, 0, 0);
    GroupExpression b(LogicalOperatorType::LOGICAL_GET, {}, 1, 0);
    CHECK(a == b, "Same type, same children should be equal");

    GroupExpression c(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {0, 1}, 2, 0);
    GroupExpression d(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {0, 1}, 3, 1);
    CHECK(c == d, "Join with same children should be equal");

    GroupExpression e(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {1, 0}, 4, 2);
    CHECK(!(c == e), "Join with swapped children should NOT be equal");

    // Hashes should match for equal expressions
    CHECK(a.hash == b.hash, "Equal expressions should have same hash");
    CHECK(c.hash == d.hash, "Equal joins should have same hash");
}

//===----------------------------------------------------------------------===//
// Test 3: Physical expression insertion and best-cost tracking
//===----------------------------------------------------------------------===//
void TestPhysicalExpression() {
    Memo memo;

    GroupID g1 = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    GroupID g2 = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});

    // Insert physical expressions into g1's group
    memo.InsertPhysical(g1, PhysicalOperatorType::TABLE_SCAN, {}, 10.0);
    memo.InsertPhysical(g1, PhysicalOperatorType::TABLE_SCAN, {}, 5.0);

    auto *best = memo.FinalizeBestPhysical(g1);
    CHECK(best != nullptr, "Should have a best physical expression");
    CHECK(best->phys_op_type == PhysicalOperatorType::TABLE_SCAN, "Best should be TABLE_SCAN");
    CHECK(best->cost == 5.0, "Best cost should be 5.0 (lower than 10.0)");

    // Insert physical expressions into g2
    memo.InsertPhysical(g2, PhysicalOperatorType::TABLE_SCAN, {}, 3.0);
    best = memo.FinalizeBestPhysical(g2);
    CHECK(best != nullptr, "g2 should have best physical");
    CHECK(best->cost == 3.0, "g2 best cost should be 3.0");

    // Insert join and its physical implementations
    GroupID j1 = memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {g1, g2});
    memo.InsertPhysical(j1, PhysicalOperatorType::HASH_JOIN, {g1, g2}, 15.0);
    memo.InsertPhysical(j1, PhysicalOperatorType::NESTED_LOOP_JOIN, {g1, g2}, 50.0);

    best = memo.FinalizeBestPhysical(j1);
    CHECK(best != nullptr, "Join group should have best physical");
    CHECK(best->phys_op_type == PhysicalOperatorType::HASH_JOIN, "Hash join should be cheaper");
    CHECK(best->cost == 15.0, "Best cost should be 15.0");
}

//===----------------------------------------------------------------------===//
// Test 4: SearchEngine with JoinCommutativityRule
//===----------------------------------------------------------------------===//
void TestJoinCommutativity() {
    Memo memo;

    // Create leaf groups
    GroupID g1 = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    GroupID g2 = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});

    // Create join
    memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {g1, g2});

    // Create search engine with join commutativity rule
    SearchEngine search(memo);
    search.AddExplorationRule(make_uniq<JoinCommutativityRule>());
    search.AddImplementationRule(make_uniq<HashJoinImplementationRule>());

    // Run search
    auto result = search.Optimize(2); // root group = group 2 (the join)

    // Verify explore phase created the swapped join alternative
    auto &root_group = memo.GetGroup(2);
    CHECK(root_group.logical_expressions.size() > 0, "Root group should have logical expressions");

    // Check if swapped join was created (should be in group 3 or later)
    bool found_swapped = false;
    for (idx_t i = 0; i < memo.GroupCount(); i++) {
        auto &group = memo.GetGroup(i);
        for (auto &expr : group.logical_expressions) {
            if (expr->op_type == LogicalOperatorType::LOGICAL_COMPARISON_JOIN &&
                expr->children.size() == 2 &&
                expr->children[0] == g2 && expr->children[1] == g1) {
                found_swapped = true;
            }
        }
    }
    CHECK(found_swapped, "Join commutativity should produce swapped join (g2 ? g1)");

    // Verify optimize phase created physical expressions
    CHECK(root_group.physical_expressions.size() > 0, "Optimize phase should create physical expressions");
    CHECK(root_group.best_physical != nullptr, "Should have chosen a best physical plan");
    CHECK(result.cost > 0, "Optimization result should have positive cost");
}

//===----------------------------------------------------------------------===//
// Test 5: Three-table join chain (multi-level Memo)
//===----------------------------------------------------------------------===//
void TestThreeTableJoin() {
    Memo memo;

    // Three leaf tables
    GroupID a = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    GroupID b = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});
    GroupID c = memo.InsertExpression(LogicalOperatorType::LOGICAL_GET, {});

    // Join A with B, then result with C
    GroupID ab = memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {a, b});
    GroupID abc = memo.InsertExpression(LogicalOperatorType::LOGICAL_COMPARISON_JOIN, {ab, c});

    CHECK(abc < memo.GroupCount(), "ABC join should be in a valid group");
    CHECK(memo.GroupCount() == 5, "Should have 5 groups: 3 leaf + AB + ABC");

    // Run search with commutativity (creates alternatives like B?A, (C?AB), etc.)
    SearchEngine search(memo);
    search.AddExplorationRule(make_uniq<JoinCommutativityRule>());
    search.AddImplementationRule(make_uniq<HashJoinImplementationRule>());

    search.Optimize(abc);

    // After explore: more expressions should exist (swapped joins)
    auto &root = memo.GetGroup(abc);
    fprintf(stderr, "  INFO: Three-table join result: %zu groups, "
            "%zu physical exprs, best_cost=%.2f\n",
            memo.GroupCount(), root.physical_expressions.size(),
            memo.GetGroup(abc).best_cost);
}

//===----------------------------------------------------------------------===//
// Test 6: Rule registration and naming
//===----------------------------------------------------------------------===//
void TestRuleRegistration() {
    JoinCommutativityRule comm_rule;
    CHECK(std::string(comm_rule.GetName()) == "Join Commutativity", "Rule name should match");

    HashJoinImplementationRule hash_rule;
    CHECK(std::string(hash_rule.GetName()) == "Hash Join Implementation", "Rule name should match");

    CHECK(comm_rule.GetType() == Rule::RuleType::EXPLORATION, "JoinCommutativity should be exploration");
    CHECK(hash_rule.GetType() == Rule::RuleType::IMPLEMENTATION, "HashJoin should be implementation");
}

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//
int main(int argc, char **argv) {
    fprintf(stderr, "\n=== Cascade CBO Core Unit Tests ===\n\n");

    TestMemoInsert();
    TestGroupExpressionEq();
    TestPhysicalExpression();
    TestJoinCommutativity();
    TestThreeTableJoin();
    TestRuleRegistration();

    fprintf(stderr, "\n=== Results: %d passed, %d failed ===\n",
            tests_passed, tests_failed);

    // Also print a summary the user can easily see
    fprintf(stdout, "\n=== Cascade CBO Core Tests Complete ===\n");
    fprintf(stdout, "  %d / %d tests passed\n", tests_passed, tests_passed + tests_failed);
    fprintf(stdout, "\nTest the SQL integration separately with:\n");
    fprintf(stdout, "  build/reldebug/test/unittest test/sql/optimizer/cascade/test_cascade.test\n");

    return tests_failed > 0 ? 1 : 0;
}
