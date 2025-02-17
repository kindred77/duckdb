#include "duckdb/optimizer/orca/search/SearchStage.hpp"

namespace duckdb {

SearchStages SearchStage::GenderateDefault() {

}

void
SearchStage::SetBestExpr(ExpressionPtr pexpr) {
	GPOS_ASSERT_IMP(nullptr != pexpr, pexpr->Pop()->FPhysical());

	m_pexprBest = pexpr;
	if (nullptr != m_pexprBest)
	{
		m_costBest = m_pexprBest->Cost();
	}
}

}
