#pragma once
#include "duckdb/optimizer/orca/XForm.hpp"

namespace duckdb {

class Cost;
using CostPtr = std::shared_ptr<Cost>;

class Cost {
public:
    // ctor
	explicit Cost(double d)
	{
	}

	// ctor
	Cost(const Cost &cost) : Cost(cost.Get())
	{
	}

	// assignment
	Cost &
	operator=(const Cost &cost)
	{
		*(CDouble *) (this) = (CDouble) cost;
		return (*this);
	}

	// addition operator
	Cost
	operator+(const Cost &cost) const
	{
		CDouble d = (CDouble)(*this) + (CDouble) cost;
		return CCost(d);
	}

	// multiplication operator
	Cost
	operator*(const Cost &cost) const
	{
		return Cost((CDouble)(*this) * (CDouble) cost);
	}

	// comparison operator
	bool
	operator<(const Cost &cost) const
	{
		return (CDouble)(*this) < (CDouble) cost;
	}

	// comparison operator
	bool
	operator>(const Cost &cost) const
	{
		return (CDouble) cost < (CDouble)(*this);
	}

	// d'tor
	virtual ~Cost() = default;
};

}
