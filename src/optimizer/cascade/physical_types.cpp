#include "duckdb/optimizer/cascade/physical_types.hpp"
namespace duckdb { namespace cascade {

string PhysicalOperatorTypeToString(PhysicalOperatorType type) {
    switch (type) {
    case PhysicalOperatorType::TABLE_SCAN: return "TABLE_SCAN";
    case PhysicalOperatorType::HASH_JOIN: return "HASH_JOIN";
    case PhysicalOperatorType::NESTED_LOOP_JOIN: return "NESTED_LOOP_JOIN";
    case PhysicalOperatorType::FILTER: return "FILTER";
    case PhysicalOperatorType::PROJECTION: return "PROJECTION";
    case PhysicalOperatorType::AGGREGATE: return "AGGREGATE";
    case PhysicalOperatorType::ORDER_BY: return "ORDER_BY";
    case PhysicalOperatorType::LIMIT: return "LIMIT";
    case PhysicalOperatorType::DISTINCT: return "DISTINCT";
    case PhysicalOperatorType::CROSS_PRODUCT: return "CROSS_PRODUCT";
    default: return "INVALID";
    }
}

}}
