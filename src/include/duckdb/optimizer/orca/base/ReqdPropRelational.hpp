#pragma once

#include "duckdb/optimizer/orca/base/ReqdProp.hpp"
#include "duckdb/optimizer/orca/base/ColRefSet.hpp"

namespace duckdb {

class ReqdPropRelational;
using ReqdPropRelationalPtr = std::shared_ptr<ReqdPropRelational>;

class ReqdPropRelational : public ReqdProp {
private:

public:
	ReqdPropRelational(const ReqdPropRelational &) = delete;

	// default ctor
	ReqdPropRelational();

	// ctor
	explicit ReqdPropRelational(ColRefSetPtr pcrs);

	// dtor
	~ReqdPropRelational();

	// type of properties
	bool
	FRelational() const override
	{
		//GPOS_ASSERT(!FPlan());
		return true;
	}

};

}