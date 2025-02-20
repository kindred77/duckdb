#include "duckdb/optimizer/orca/Expression.hpp"

namespace duckdb {

Expression::Expression(LogicalOperatorPtr logical_operator,
				GroupExpressionPtr group_expression = nullptr)
	: operator_ptr(std::move(logical_operator))
	, group_expression(std::move(group_expression)) {
    
}

}
