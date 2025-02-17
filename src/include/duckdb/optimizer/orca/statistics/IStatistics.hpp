#pragma once

namespace duckdb {

class IStatistics;
using IStatisticsPtr = std::shared_ptr<IStatistics>;

class IStatistics {
public:
	IStatistics &operator=(IStatistics &) = delete;

	IStatistics(const IStatistics &) = delete;

	enum EStatsJoinType {
		EsjtInnerJoin,
		EsjtLeftOuterJoin,
		EsjtLeftSemiJoin,
		EsjtLeftAntiSemiJoin,
		EstiSentinel  // should be the last in this enum
	};

	// ctor
	IStatistics() = default;

	// dtor
	~IStatistics() = default;

	// how many rows
	virtual double Rows() const = 0;
};

// dynamic array for derived stats
using IStatisticsArray = std::vector<IStatisticsPtr>;

}
