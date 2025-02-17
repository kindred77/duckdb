#pragma once
#include "duckdb/optimizer/orca/XForm.hpp"
#include "duckdb/optimizer/orca/cost/Cost.hpp"
#include "duckdb/optimizer/orca/common/Timer.hpp"

namespace duckdb {

class TimerUser : public Timer{
private:
	// actual timer
	//RUSAGE m_rusage;

public:
	// ctor
	TimerUser() = default;

	// retrieve elapsed user time in micro-seconds
	uint32_t ElapsedUS() const override;

	// restart timer
	void Restart() override;
};

}
