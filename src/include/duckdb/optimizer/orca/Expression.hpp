#pragma once

#include "duckdb/optimizer/orca/search/GroupExpression.hpp"
#include "duckdb/planner/logical_operator.hpp"

namespace duckdb {

class Expression;
using ExpressionPtr = std::shared_ptr<Expression>;
using ExpressionPtrs = std::vector<ExpressionPtr>;
using LogicalOperatorPtr = std::shared_ptr<LogicalOperator>;

class Expression {
private:
    LogicalOperatorPtr m_pop;
    std::shared_ptr<ExpressionPtrs> m_pdrgpexpr;

	// group reference to Memo
    GroupExpressionPtr m_pgexpr;
public:
    Expression(LogicalOperatorPtr logical_operator,
				GroupExpressionPtr pgexpr = nullptr);

    ExpressionPtr
	operator[](size_t idx) const {
		//GPOS_ASSERT(nullptr != m_pdrgpexpr);
		return m_pdrgpexpr->operator[](idx);
	};

    size_t
	Arity() const {
		return m_pdrgpexpr == nullptr ? 0 : expressions->size();
	}

    LogicalOperatorPtr
	Pop() const {
		//GPOS_ASSERT(nullptr != m_pop);
		return m_pop;
	}

	// accessor of children array
	std::shared_ptr<ExpressionPtrs> PdrgPexpr() const {
		return m_pdrgpexpr;
	}

	// accessor for origin group expression
	GroupExpressionPtr Pgexpr() const {
		return m_pgexpr;
	}
};

}