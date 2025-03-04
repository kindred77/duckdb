#pragma once


#define GPOS_USEC_IN_MSEC ((uint64_t) 1000)

namespace duckdb {

class Timer {
private:
public:
	Timer(const Timer &) = delete;

	// ctor
	Timer() = default;

	// dtor
	virtual ~Timer() = default;

	// retrieve elapsed time in micro-seconds
	virtual uint32_t ElapsedUS() const = 0;

	// retrieve elapsed time in milli-seconds
	uint32_t
	ElapsedMS() const
	{
		return ElapsedUS() / GPOS_USEC_IN_MSEC;
	}

	// restart timer
	virtual void Restart() = 0;
};

}
