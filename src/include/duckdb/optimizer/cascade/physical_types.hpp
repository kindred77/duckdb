//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/optimizer/cascade/physical_types.hpp
//
//===----------------------------------------------------------------------===//
#pragma once
#include "duckdb/common/common.hpp"
namespace duckdb { namespace cascade {

enum class PhysicalOperatorType : uint8_t {
    INVALID = 0,
    TABLE_SCAN,
    HASH_JOIN,
    NESTED_LOOP_JOIN,
    FILTER,
    PROJECTION,
    AGGREGATE,
    ORDER_BY,
    LIMIT,
    DISTINCT,
    CROSS_PRODUCT,
};

string PhysicalOperatorTypeToString(PhysicalOperatorType type);

}}
