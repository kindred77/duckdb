#pragma once
#include "duckdb/optimizer/orca/XForm.hpp"
#include "duckdb/optimizer/orca/cost/Cost.hpp"
#include "duckdb/optimizer/orca/Expression.hpp"
#include "duckdb/optimizer/orca/common/TimerUser.hpp"

namespace duckdb {

class SearchStage;
using SearchStagePtr = std::shared_ptr<SearchStage>;
using SearchStages = std::vector<SearchStagePtr>;

class SearchStage {
private:
    // set of xforms to be applied during stage
	//XFormSetPtr m_xforms;

	// time threshold in milliseconds
	uint32_t m_time_threshold;

	// cost threshold
	Cost m_cost_threshold;

	// best plan found at the end of search stage
	ExpressionPtr m_pexprBest;

	// cost of best plan found
	Cost m_costBest;

	// elapsed time
	TimerUser m_timer;
public:
    static SearchStages GenderateDefault();

    // ctor
	SearchStage(XFormSetPtr xform_set, uint32_t ulTimeThreshold = gpos::ulong_max,
				 CCost costThreshold = CCost(0.0));

	// dtor
	virtual ~SearchStage();

    // restart timer if time threshold is not default indicating don't timeout
	// Restart() is a costly method, so avoid calling unnecessarily
	void
	RestartTimer()
	{
		if (m_time_threshold != gpos::ulong_max)
		{
			m_timer.Restart();
		}
	}

    // set best plan found at the end of search stage
	void SetBestExpr(ExpressionPtr pexpr);
};

}
